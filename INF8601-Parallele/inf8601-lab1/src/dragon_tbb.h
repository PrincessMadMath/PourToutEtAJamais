/*
 * dragon_tbb.h
 *
 *  Created on: 2011-08-17
 *      Author: francis
 */

#ifndef DRAGON_TBB_H_
#define DRAGON_TBB_H_

#include "dragon.h"

#ifdef __cplusplus
extern "C" {
#endif
int dragon_draw_tbb(char **canvas, struct rgb *image, int width, int height, uint64_t size, int nb_thread);
int dragon_limits_tbb(limits_t *limits, uint64_t size, int nb_thread);
#ifdef __cplusplus
}
#endif

#endif /* DRAGON_TBB_H_ */
