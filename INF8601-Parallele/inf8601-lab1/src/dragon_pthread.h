/*
 * dragon_pthread.h
 *
 *  Created on: 2011-08-17
 *      Author: francis
 */

#ifndef DRAGON_PTHREAD_H_
#define DRAGON_PTHREAD_H_

#include "dragon.h"

int dragon_draw_pthread(char **canvas, struct rgb *image, int width, int height, uint64_t size, int nb_thread);
int dragon_limits_pthread(limits_t *lim, uint64_t size, int nb_thread);

#endif /* DRAGON_PTHREAD_H_ */
