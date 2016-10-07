/*
 * layer.h
 *
 *  Created on: 2011-10-07
 *      Author: francis
 */

#ifndef LAYER_H_
#define LAYER_H_

#include <png.h>

#include "stripe.h"

#define HEADER_SIZE 8

enum chan_rgb {
	CHAN_RED,
	CHAN_GREEN,
	CHAN_BLUE,
	CHAN_ALPHA,
};

typedef struct layers layers_t;

struct layers {
	stripe_array_t **layers;
	int len;
};

enum sim_layers {
	LAYER_HEAT = 0,
	LAYER_DIFF = 1,
	LAYER_SINK = 2,
	LAYER_MAT1 = 3,
	LAYER_MAT2 = 4
};

struct png_image {
	png_bytep* rows;
	int width;
	int height;
};

void free_layers(layers_t *layers);
layers_t *make_layers(int len);
void free_png(struct png_image *image);
struct png_image *make_png(int width, int height, int rowbytes);
float sum(float *m, int width, int height);
void dump_layer(float *m, int width, int height);
FILE *open_png(char *path);
void dump_png(png_bytep *rows_ptr, int width, int height);
struct png_image *load_png(char *png);
int load_mask(struct png_image *image, int chan, stripe_t *mask);
void mul_layer(layers_t *layers, float factor);
void reset_layers(layers_t *layer, float value);
int init_layers(char *path, layers_t **layers);
int alloc_layers(layers_t *layers, int width, int height);
void dump_layers(layers_t *l);
void split_layers(layers_t *layers, int nb);
void merge_layers(layers_t *layers);

#endif /* LAYER_H_ */
