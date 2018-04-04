/*
 * ssl.xc
 *
 *  Created on: 2018Äê1ÔÂ15ÈÕ
 *      Author: Brick
 */
#include "ssl.h"
// for printf
#include <stdio.h>
// for int32_t
#include <stdint.h>
// for scope
#include <xscope.h>
// for memcpy
#include <string.h>
// for malloc
#include <stdlib.h>

#include "tdoa.h"
#include "particle.h"
#include "fft.h"
#include "srp.h"
#include "time.h"

void wav2frame(streaming chanend c_wav, server interface wav_frame_if i_frame, streaming chanend c_frame)
{
    int32_t in_samps[4] = {0};
    int to_frame = 0;
    while(1){
        if(to_frame == 0){
            c_wav :> in_samps[1];
            c_wav :> in_samps[3];
            c_wav :> in_samps[2];
            c_wav :> in_samps[0];
        }
        else{
            to_frame = 0;
            for(size_t i=0; i<FRAME_SIZE; i++){
                c_wav :> in_samps[1];       // 0 data of second mic
                c_frame <: (int)(in_samps[1]*1);
                c_wav :> in_samps[3];       // 1 data of third mic
                c_frame <: in_samps[3];
                c_wav :> in_samps[2];       // 2 data of second mic
                c_frame <: (int)(in_samps[2]/1);
                c_wav :> in_samps[0];       // 3 data of four mic
                c_frame <: in_samps[0];
//                printf("send one MIC ok!! %d\n", i);
            }
//            printf("send one frame ok!!\n");
        }

        xscope_int(CH_0, in_samps[0]);
        xscope_int(CH_1, (int)(in_samps[1]*1));
        xscope_int(CH_2, (int)(in_samps[2]/1));
        xscope_int(CH_3, in_samps[3]);
//        printf("%d\n", in_samps[0]);
        select{
            case i_frame.get_frame_data():
                to_frame = 1;
                break;
            default:
                break;
        }
    }
}

void ssl_loopback(
        client interface audio_callback_if i_audio_server,
        client interface tdoa_callback_if i_tdoa_server)
{
    printf("starting ssl...\n");
    int32_t ssl_count = 0;
    // location by index, so use int8_t
    int8_t particle_location[POPULATION_NUM][3];
    float particle_speed[POPULATION_NUM][3];
    int8_t best_location[POPULATION_NUM][3];
//    int8_t best_fitness[POPULATION_NUM][3];
//    int8_t best_location_in_all[3];

//    int32_t location_index;
    clock_t tstart_time, tend_time, start_time, end_time;
    tstart_time = clock();
    i_tdoa_server.create_tdoa_table();
    tend_time = clock();
    printf("create_tdoa_table, take time:%f\n", (float)(tend_time-tstart_time)/CLOCKS_PER_SEC);
    while(1){
        printf("-------- ssl count: %d\n", ssl_count++);
        start_time = clock();
        tstart_time = clock();
        init_particle(particle_location, particle_speed);
        tend_time = clock();
        printf("init_particle, take time:%f\n", (float)(tend_time-tstart_time)/CLOCKS_PER_SEC);
        memcpy(best_location, particle_location, sizeof(int8_t)*POPULATION_NUM*3);

        tstart_time = clock();
        i_audio_server.get_wav_frame();
//        i.extract_audio_frame();
        i_audio_server.srp_formulate();
        tend_time = clock();
        printf("process audio, take time:%f\n", (float)(tend_time-tstart_time)/CLOCKS_PER_SEC);
        aPOINT sound_point;
        cPOINT tmp_point;
        pPOINT ppoint;
        int16_t same_srp_count = 0;
        tstart_time = clock();
        while(1){
//            location_index = update_particle(
//                    particle_location, particle_speed, best_fitness, best_location_in_all);
            float max_srp = 0;
            for(int32_t i=0; i<SEARCH_POINT; i++){
                int8_t td0, td1, td2, td3, td4, td5;
                {td0, td1, td2, td3, td4, td5} = i_tdoa_server.get_tdoa_data(i);
                float srp_phat = i_audio_server.caculate_srp(td0, td1, td2, td3, td4, td5);
//                printf("get_tdoa_data:%d --> %d, %d, %d, %d, %d, %d -- > srp_phat: %f\n", i, td0, td1, td2, td3, td4, td5, srp_phat);
//                printf("%f, ", srp_phat);
                if(srp_phat > max_srp){
                    max_srp = srp_phat;
                    tmp_point = index2xyz(i);
                    sound_point.x = tmp_point.x;
                    sound_point.y = tmp_point.y;
                    sound_point.z = tmp_point.z;
                    same_srp_count = 1;
                } else if(srp_phat == max_srp){
                    tmp_point = index2xyz(i);
                    ppoint = cart_to_sph(tmp_point);
//                    printf("sound_point, x:%d\ty:%d\tz:%d\tsph, theta:%d\tphi:%d\tr:%d\tmax_srp: %f\n",
//                            tmp_point.x, tmp_point.y, tmp_point.z, ppoint.theta, ppoint.phi, ppoint.r, max_srp);
                    sound_point.x += tmp_point.x;
                    sound_point.y += tmp_point.y;
                    sound_point.z += tmp_point.z;
                    same_srp_count++;
                }
            }
            tmp_point.x = sound_point.x / same_srp_count;
            tmp_point.y = sound_point.y / same_srp_count;
            tmp_point.z = sound_point.z / same_srp_count;
//            printf("sound_point, x:%d, y:%d, z:%d, max_srp: %f, same_srp_count:%d\n",
//                    tmp_point.x, tmp_point.y, tmp_point.z, max_srp, same_srp_count);
//            printf("index2xyz, x:%d, y:%d, z:%d\n", point.x, point.y, point.z);
            ppoint = cart_to_sph(tmp_point);
//            printf("sound location: theta: %d, phi:%d, r:%d\n", ppoint.theta, ppoint.phi, ppoint.r);
            printf("sound location: theta: %d, r:%d\n", ppoint.theta, ppoint.r);
//            i_audio_server.ssl_is_ok();
//            i_audio_server.get_results();
//            i_audio_server.update_particle();
            break;
        }
        tend_time = clock();
        printf("location estimation, take time:%f\n", (float)(tend_time-tstart_time)/CLOCKS_PER_SEC);
        end_time = clock();
        printf("this ssl take time:%f\n", (float)(end_time-start_time)/CLOCKS_PER_SEC);
//        break;
    }
}

void tdoa_server(server interface tdoa_callback_if i)
{
    // notice: MIC_PAIR = row
    printf("MIC_PAIR:%d, SEARCH_POINT:%d\n", MIC_PAIR, SEARCH_POINT);
    int8_t TDOA_table[MIC_PAIR][SEARCH_POINT];      // tdoa table data

    while(1){
        select{
            case i.create_tdoa_table():
                create_tdoa_table(TDOA_table);
                break;
            case i.get_tdoa_data(int32_t index) ->
                    {int8_t td0, int8_t td1, int8_t td2, int8_t td3, int8_t td4, int8_t td5}:
                td0 = TDOA_table[0][index];
                td1 = TDOA_table[1][index];
                td2 = TDOA_table[2][index];
                td3 = TDOA_table[3][index];
                td4 = TDOA_table[4][index];
                td5 = TDOA_table[5][index];
//                printf("get_tdoa_data\n");
                break;
        }
    }
}

void audio_server(
        server interface audio_callback_if i,
        client interface wav_frame_if i_frame,
        streaming chanend c_frame)
{
    // notice: MIC_PAIR = row
    printf("MIC_PAIR:%d, SEARCH_POINT:%d\n", MIC_PAIR, SEARCH_POINT);

    complex enframe_data[MIC][FRAME_SIZE];
    complex R[MIC_PAIR][FRAME_SIZE];
    memset(enframe_data, 0, sizeof(complex)*MIC*FRAME_SIZE);
    memset(R, 0, sizeof(complex)*MIC*FRAME_SIZE);

    int tmp;
    while(1){
        select{
            case i.get_wav_frame():
                i_frame.get_frame_data();
                for(size_t i=0; i<FRAME_SIZE; i++){
                    for(size_t j=0; j<MIC; j++){
                        // enframe_data[j][i].real is float type, here should be int
                        c_frame :> tmp;
                        enframe_data[j][i].real = tmp;
                    }
//                    printf("receive one MIC ok!! %d\n", i);
//                    xscope_int(CH_0, enframe_data[0][i].real);
//                    xscope_int(CH_1, enframe_data[1][i].real);
//                    xscope_int(CH_2, enframe_data[2][i].real);
//                    xscope_int(CH_3, enframe_data[3][i].real);
                }
//                printf("receive one frame ok!!\n");
                break;
            case i.extract_audio_frame():
                // vad
                printf("extract_audio_frame\n");
                break;
            case i.srp_formulate():
                caculate_gccphat(enframe_data, R);
//                for(int8_t i=0; i<MIC_PAIR; i++){
//                    for(int32_t j=0; j<FRAME_SIZE; j++){
//                        printf("R[%d][%d]: %f\n", i, j, R[i][j].real);
//                    }
//                }
                break;
            case i.caculate_srp(int8_t td0, int8_t td1, int8_t td2,
                    int8_t td3, int8_t td4, int8_t td5) -> float srp_local:
                int center = (FRAME_SIZE/2)-1;
                srp_local = 0;
                srp_local = srp_local + R[0][td0 + center].real;
                srp_local = srp_local + R[1][td1 + center].real;
                srp_local = srp_local + R[2][td2 + center].real;
                srp_local = srp_local + R[3][td3 + center].real;
                srp_local = srp_local + R[4][td4 + center].real;
                srp_local = srp_local + R[5][td5 + center].real;
//                printf("srp_local: %f\n", srp_local);
//                int index = find_source_location(TDOA_table, R);
//                printf("caculate_srp:%d\n", index);
                break;
        }
    }
}
