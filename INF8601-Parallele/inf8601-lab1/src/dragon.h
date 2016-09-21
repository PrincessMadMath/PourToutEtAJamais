/*
 * dragon.h
 *
 *  Created on: 2011-06-26
 *      Author: francis
 */

#ifndef DRAGON_H_
#define DRAGON_H_

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include "color.h"

/**
 * TODO:
 *
 * macro to flag unimplemented blocks
 */
#define TODO(msg) 								\
    fprintf(stderr, "%s:%d Unimplemented block %s\n",			\
            __FILE__, __LINE__, msg);

#define FREE(var) do { 	\
	if (var != NULL) { 	\
		free(var);		\
		var = NULL;		\
	}					\
} while(0)

typedef struct xy_ {
	int64_t	x;
	int64_t y;
} xy_t;

typedef struct limites_ {
	xy_t	minimums;
	xy_t	maximums;
} limits_t;

typedef struct morceau_ {
	xy_t		position;
	xy_t		orientation;
	limits_t	limits;
} piece_t;

struct draw_data {
	int id;
	int *tid;
	int nb_thread;
	int dragon_width;
	int dragon_height;
	int image_width;
	int image_height;
	int scale;
	int deltaI;
	int deltaJ;
	struct rgb *image;
	struct palette *palette;
	char *dragon;
	uint64_t size;
	limits_t limits;
	pthread_barrier_t *barrier;
//};
} __attribute__((aligned(128)));

struct limit_data {
	int id;
	uint64_t start;
	uint64_t end;
	piece_t piece;
//};
} __attribute__((aligned(128)));

int dragon_limits_serial(limits_t *limits, uint64_t nbIterations, int nb_thread);
void dump_limits(limits_t *limits);
int cmp_limits(limits_t *l1, limits_t *l2);
void piece_limit(int64_t debut, int64_t fin, piece_t *m);
void piece_merge(piece_t *m1, piece_t m2);
void piece_init(piece_t *piece);
void rotate_left(xy_t *xy);
void rotate_right(xy_t *xy);
void limits_invert(limits_t *limites);
xy_t compute_position(int64_t i);
xy_t compute_orientation(int64_t i);
int dragon_draw_serial(char **dragon, struct rgb *image, int width, int height, uint64_t size, __attribute__((unused)) int nb_thread);
void dump_canvas(char *canvas, int width, int height);
void dump_canvas_rgb(struct rgb *canvas, int width, int height);
int write_img(struct rgb *image, char *file, int width, int height);
struct rgb *make_canvas(int width, int height);
int cmp_canvas(char *exp, char *act, int width, int height, int verbose);
void init_canvas(int start, int end, char *canvas, char value);
void scale_dragon(int start, int end, struct rgb *image, int image_width, int image_height,
        char *dragon, int dragon_width, int dragon_height, struct palette *palette);
int dragon_draw_raw(uint64_t start, uint64_t end, char *dragon, int width, int height, limits_t limits, char id);

#endif /* DRAGON_H_ */
