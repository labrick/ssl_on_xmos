
#ifndef TDOA_H
#define TDOA_H
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include "common.h"

// cart point struct
typedef struct CART_POINT{
    int16_t x;          // mm
    int16_t y;          // mm
    int16_t z;          // mm
}cPOINT;
// for accuratee calculation
typedef struct CART_APOINT{
    float x;          // mm
    float y;          // mm
    float z;          // mm
}aPOINT;
// polar point struct
typedef struct POLAR_POINT{
    int8_t theta;           // du
    int8_t phi;             // du
    int16_t r;              // mm
}pPOINT;

void create_tdoa_table(int8_t TDOA_table[MIC_PAIR][SEARCH_POINT]);
float calulate_distance(aPOINT x1 ,aPOINT x2);

#ifdef USE_CARTESIAN_COORDINATE
    cPOINT index2xyz(int16_t origin_index);
    int16_t xyz2index(cPOINT point);
#else
    cPOINT sph_to_cart(pPOINT ppoint);
    pPOINT index2tpr(int16_t origin_index);
    int16_t tpr2index(pPOINT point);
#endif

#endif /* TDOA_H */
