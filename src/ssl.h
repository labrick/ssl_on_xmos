/*
 * ssl.xc
 *
 *  Created on: 2018Äê1ÔÂ15ÈÕ
 *      Author: Brick
 */

#ifndef SSL_H_
#define SSL_H_

#include <stdint.h>

//void ssl();

interface tdoa_callback_if {
    void create_tdoa_table();
    {int8_t, int8_t, int8_t, int8_t, int8_t, int8_t}
        get_tdoa_data(int32_t index);
};

interface audio_callback_if {
    // 2048 point multipy by num of mics
    void get_wav_frame();
    // vad and extract audio frame
    void extract_audio_frame();
    // preprocess audio to srp formulate
    void srp_formulate();

    // loop function
    // caculte srp of particle
    float caculate_srp(int8_t td0, int8_t td1, int8_t td2,
            int8_t td3, int8_t td4, int8_t td5);
};

interface wav_frame_if {
    void get_frame_data();
};

void audio_server(
        server interface audio_callback_if i,
        client interface wav_frame_if i_frame,
        streaming chanend c_frame);
void tdoa_server(
        server interface tdoa_callback_if i);
void ssl_loopback(
        client interface audio_callback_if i_audio_server,
        client interface tdoa_callback_if i_tdoa_server
        );
void wav2frame(streaming chanend c_wav, server interface wav_frame_if i_frame, streaming chanend c_frame);

#endif /* SSL_H_ */
