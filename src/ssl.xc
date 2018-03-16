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

void wav2frame(streaming chanend c_wav, server interface wav_frame_if i_frame, streaming chanend c_frame)
{
    int32_t in_samps[4] = {0};
    int to_frame = 0;
    while(1){
        if(to_frame == 0){
            for (size_t j = 0; j < 4; j++) {
                c_wav :> in_samps[j];
            }
        }
        else{
            to_frame = 0;
            for(size_t i=0; i<FRAME_SIZE; i++){
                for(size_t j=0; j<4; j++){
                    c_wav :> in_samps[j];
                    c_frame <: in_samps[j];
//                    printf("%d\n", in_samps[j]);
                }
            }
        }

//        xscope_int(CH_0, in_samps[0]);
//        xscope_int(CH_1, in_samps[1]);
//        xscope_int(CH_2, in_samps[2]);
//        xscope_int(CH_3, in_samps[3]);
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
    // location by index, so use int8_t
    int8_t particle_location[POPULATION_NUM][3];
    float particle_speed[POPULATION_NUM][3];
    int8_t best_location[POPULATION_NUM][3];
//    int8_t best_fitness[POPULATION_NUM][3];
//    int8_t best_location_in_all[3];

//    int32_t location_index;

    i_tdoa_server.create_tdoa_table();
    while(1){
        printf("init_particle\n");
        init_particle(particle_location, particle_speed);
        memcpy(best_location, particle_location, sizeof(int8_t)*POPULATION_NUM*3);

        i_audio_server.get_wav_frame();
//        i.extract_audio_frame();
        i_audio_server.srp_formulate();
        while(1){
//            location_index = update_particle(
//                    particle_location, particle_speed, best_fitness, best_location_in_all);
            float max_srp = 0;
            int32_t max_srp_index = 0;
            for(int32_t i=0; i<SEARCH_POINT; i++){
                int8_t td0, td1, td2, td3, td4, td5;
                {td0, td1, td2, td3, td4, td5} = i_tdoa_server.get_tdoa_data(i);
                float srp_phat = i_audio_server.caculate_srp(td0, td1, td2, td3, td4, td5);
                if(srp_phat > max_srp){
                    max_srp = srp_phat;
                    max_srp_index = i;
                }
            }
            printf("max_srp_index: %d, max_srp: %f\n", max_srp_index, max_srp);
            cPOINT point = index2xyz(max_srp_index);
            printf("max_srp_index to xyz: %d, %d, %d\n", point.x, point.y, point.z);
//            i_audio_server.ssl_is_ok();
//            i_audio_server.get_results();
//            i_audio_server.update_particle();
            break;
        }
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
                printf("create_tdoa_table\n");
                create_tdoa_table(TDOA_table);
//                for(int j=0; j<SEARCH_POINT; j++){
//                    printf("\n %d \n", j);
//                    for(int i=0; i<MIC_PAIR; i++){
//                        printf("%d\t", TDOA_table[i][j]);
//                    }
//                }
                break;
            case i.get_tdoa_data(int32_t index) ->
                    {int8_t td0, int8_t td1, int8_t td2, int8_t td3, int8_t td4, int8_t td5}:
                td0 = TDOA_table[0][index];
                td1 = TDOA_table[1][index];
                td2 = TDOA_table[2][index];
                td3 = TDOA_table[3][index];
                td4 = TDOA_table[4][index];
                td5 = TDOA_table[5][index];
                printf("init_particle\n");
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
    float R[MIC_PAIR][FRAME_SIZE];

    while(1){
        select{
            case i.get_wav_frame():
                printf("get_wav_frame\n");
                i_frame.get_frame_data();
                for(size_t i=0; i<FRAME_SIZE; i++){
                    for(size_t j=0; j<MIC; j++){
                        c_frame :> enframe_data[j][i].real;
//                        printf("------%d\n", enframe_data[j][i]);
                    }
//                    xscope_int(CH_0, enframe_data[0][i]);
//                    xscope_int(CH_1, enframe_data[1][i]);
//                    xscope_int(CH_2, enframe_data[2][i]);
//                    xscope_int(CH_3, enframe_data[3][i]);
                }
                break;
            case i.extract_audio_frame():
                // vad
                printf("extract_audio_frame\n");
                break;
            case i.srp_formulate():
                caculate_gccphat(enframe_data, R);
                printf("srp_formulate\n");
                break;
            case i.caculate_srp(int8_t td0, int8_t td1, int8_t td2,
                    int8_t td3, int8_t td4, int8_t td5) -> float srp_local:
                int center = (FRAME_SIZE/2)-1;
                srp_local = srp_local + R[0][td0 + center];
                srp_local = srp_local + R[1][td1 + center];
                srp_local = srp_local + R[2][td2 + center];
                srp_local = srp_local + R[3][td3 + center];
                srp_local = srp_local + R[4][td4 + center];
                srp_local = srp_local + R[5][td5 + center];
//                int index = find_source_location(TDOA_table, R);
//                printf("caculate_srp:%d\n", index);
                break;
        }
    }
}
