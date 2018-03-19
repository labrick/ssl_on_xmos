/*
 * common.h
 *
 *  Created on: 2018年1月24日
 *      Author: Brick
 */
// for printf
#include <stdio.h>

#ifndef COMMON_H_
#define COMMON_H_

/**********************
* config the srp-phat */
/**********************/
// 每一帧数据个数
#define FRAME_SIZE 1024
#define MIC 4                   //麦克风数量
#define MIC_PAIR 6


#define FS  16000               //采样率
#define C  340000               //声速
#ifndef M_PI
    #define M_PI    3.1415926535897932384626433832795f
#endif

#define USE_CARTESIAN_COORDINATE

#ifdef USE_CARTESIAN_COORDINATE
    // square search
    #define SEARCH_RANGE_X 6000         // mm
    #define SEARCH_RANGE_Y 6000         // mm
    #define SEARCH_RANGE_Z 3000         // mm

    #define SEARCH_X_STEP 150           // theta step, du
    #define SEARCH_Y_STEP 150           // phi step, du
    #define SEARCH_Z_STEP 300           // R step, mm

    #define SEARCH_X_NUM ((SEARCH_RANGE_X / SEARCH_X_STEP) + 1)
    #define SEARCH_Y_NUM ((SEARCH_RANGE_Y / SEARCH_Y_STEP) + 1)
    #define SEARCH_Z_NUM ((SEARCH_RANGE_Z / SEARCH_Z_STEP) + 1)

    #define SEARCH_POINT (SEARCH_X_NUM * SEARCH_Y_NUM * SEARCH_Z_NUM)
#else
    // cycle search
    #define SEARCH_RANGE_THETA 360        // du
    #define SEARCH_RANGE_PHI 90           // du
    #define SEARCH_RANGE_R 320            // cm

    #define SEARCH_THETA_STEP 2              // theta step, du
    #define SEARCH_PHI_STEP 5                // phi step, du
    #define SEARCH_R_STEP 40                 // R step, cm

    #define SEARCH_THETA_NUM (SEARCH_RANGE_THETA / SEARCH_THETA_STEP)
    #define SEARCH_PHI_NUM (SEARCH_RANGE_PHI / SEARCH_PHI_STEP)
    #define SEARCH_R_NUM (SEARCH_RANGE_R / SEARCH_R_STEP)

    #define SEARCH_POINT (SEARCH_THETA_NUM * SEARCH_PHI_NUM * SEARCH_R_NUM)
#endif /* USE_CARTESIAN_COORDINATE */

#endif /* COMMON_H_ */
