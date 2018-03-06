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


void ssl_loopback(client interface ssl_callback_if i)
{
    i.create_tdoa_table();
    while(1){
        printf("hello world!\n");
        i.init_particle();
        i.get_wav_frame();
//        i.extract_audio_frame();
        i.srp_formulate();
        while(1){
            i.caculate_srp();
            i.ssl_is_ok();
            i.get_results();
            i.update_particle();
            break;
        }
//        break;
    }
}

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

void ssl_implement(
        server interface ssl_callback_if i,
        client interface wav_frame_if i_frame,
        streaming chanend c_frame)
{
    // notice: MIC_PAIR = row
    printf("MIC_PAIR:%d, SEARCH_POINT:%d\n", MIC_PAIR, SEARCH_POINT);
    int8_t TDOA_table[MIC_PAIR][SEARCH_POINT];      // tdoa table data

    // location by index, so use int8_t
    int8_t particle_location[POPULATION_NUM][3];
    float particle_speed[POPULATION_NUM][3];
    int8_t best_location[POPULATION_NUM][3];
//    int8_t best_fitness[POPULATION_NUM][3];
//    int8_t best_location_in_all[3];

    int32_t enframe_data[MIC][FRAME_SIZE];
    float R[MIC_PAIR][FRAME_SIZE];

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
            case i.init_particle():
                printf("init_particle\n");
                init_particle(particle_location, particle_speed);
//                for(int i=0; i<POPULATION_NUM; i++){
//                    printf("\n %d: --------\n", i);
//                    for(int j=0; j<3; j++){
//                        printf("%d, %f\t", particle_location[i][j], particle_speed[i][j]);
//                    }
//                }
                memcpy(best_location, particle_location, sizeof(int8_t)*POPULATION_NUM*3);
                break;
            case i.get_wav_frame():
                printf("get_wav_frame\n");
                i_frame.get_frame_data();
//                while(1)
                for(size_t i=0; i<FRAME_SIZE; i++){
                    for(size_t j=0; j<MIC; j++){
                        c_frame :> enframe_data[j][i];
//                        printf("------%d\n", enframe_data[j][i]);
                    }
                    xscope_int(CH_0, enframe_data[0][i]);
                    xscope_int(CH_1, enframe_data[1][i]);
                    xscope_int(CH_2, enframe_data[2][i]);
                    xscope_int(CH_3, enframe_data[3][i]);
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
            case i.caculate_srp():
                int index = find_source_location(TDOA_table, R);
                printf("caculate_srp:%d\n", index);
                break;
            case i.ssl_is_ok():
                printf("ssl_is_ok\n");
                break;
            case i.get_results():
                printf("get_results\n");
                break;
            case i.update_particle():
                printf("update_particle\n");
                break;
        }
    }
}
