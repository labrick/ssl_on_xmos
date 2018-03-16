/*
 * particle.h
 *
 *  Created on: 2018Äê1ÔÂ16ÈÕ
 *      Author: Brick
 */

#include "tdoa.h"

#ifndef PARTICLE_H_
#define PARTICLE_H_

#define POPULATION_NUM 30
#define MAX_GENERATION 100

// define by num=distance/step, not distance
// boundary
#define XLIMIT (SEARCH_X_NUM/2)
#define YLIMIT (SEARCH_Y_NUM/2)
#define ZLIMIT SEARCH_Z_NUM

// notice: speed vlimit cannot < 1
#define VXLIMIT (XLIMIT/10)
#define VYLIMIT (YLIMIT/10)
#define VZLIMIT (ZLIMIT/5)

// pso parameter

void init_particle(
        int8_t particle_location[POPULATION_NUM][3],
        float particle_speed[POPULATION_NUM][3]);
int32_t update_particle(
        int8_t particle_location[POPULATION_NUM][3],
        float particle_speed[POPULATION_NUM][3],
        int8_t best_fitness[POPULATION_NUM][3],
        int8_t best_location_in_all[3]);
#endif /* PARTICLE_H_ */
