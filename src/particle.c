/*
 * particle.c
 *
 *  Created on: 2018Äê1ÔÂ16ÈÕ
 *      Author: Brick
 */

#include "particle.h"
#include "stdio.h"
#include <stdlib.h>
#include <time.h>
//#include "tdoa.h"

#define X 0
#define Y 1
#define Z 2
#define N 10000  // for 0~1 rand, four valid num

void init_particle(int8_t particle_location[POPULATION_NUM][3], float particle_speed[POPULATION_NUM][3])
{
    srand(time(NULL));
    cPOINT point;
    for(int i=0; i<POPULATION_NUM; i++){
//        printf("%d\n", rand());
        // location
        int16_t index = rand() % SEARCH_POINT;
        point = index2xyz(index);   // real
        particle_location[i][X] = point.x / SEARCH_X_STEP;
        particle_location[i][Y] = point.y / SEARCH_Y_STEP;
        particle_location[i][Z] = point.z / SEARCH_Z_STEP;

        // speed
        particle_speed[i][X] = (rand() % N)/(float)N;
        particle_speed[i][Y] = (rand() % N)/(float)N;
        particle_speed[i][Z] = (rand() % N)/(float)N;
    }
}

int32_t update_particle(
        int8_t particle_location[POPULATION_NUM][3],
        float particle_speed[POPULATION_NUM][3],
        int8_t best_fitness[POPULATION_NUM][3],
        int8_t best_location_in_all[3])
{
    static int32_t index = -1;
    index++;
    return index;
}
