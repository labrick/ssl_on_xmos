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

interface ssl_callback_if {
    void create_tdoa_table();
    void init_particle();
    // 2048 point multipy by num of mics
    void get_wav_frame();
    // vad and extract audio frame
    void extract_audio_frame();
    // preprocess audio to srp formulate
    void srp_formulate();

    // loop function
    // caculte srp of particle
    void caculate_srp();
    // ssl is ok?
    void ssl_is_ok();
//    int ssl_is_ok();
    // theta, phi, R
//    {float, float, float} get_results();
    void get_results();
    void update_particle();
};

interface wav_frame_if {
    void get_frame_data();
};

void ssl_implement(
        server interface ssl_callback_if i,
        client interface wav_frame_if i_frame,
        streaming chanend c_frame);
void ssl_loopback(client interface ssl_callback_if i);
void wav2frame(streaming chanend c_wav, server interface wav_frame_if i_frame, streaming chanend c_frame);

#endif /* SSL_H_ */
