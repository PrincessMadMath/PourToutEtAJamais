/*
 * image.h
 *
 *  Created on: 2011-11-17
 *      Author: francis
 */

#ifndef IMAGE_H_
#define IMAGE_H_

#include <png.h>
#include "grid.h"

#define HEADER_SIZE 8

enum chan_rgba {
	CHAN_RED,
	CHAN_GREEN,
	CHAN_BLUE,
	CHAN_ALPHA,
};

typedef struct image {
	png_bytep* rows;
	int width;
	int height;
} image_t;

FILE *open_png(char *path);
image_t *make_image(int width, int height, int rowbytes);
void free_image(image_t *image);
image_t *load_png(char *png);
grid_t *grid_from_image(image_t *image, int chan);
int save_grid_png(grid_t *grid, char *output);
void dump_image(image_t *image, int chan);

#endif /* IMAGE_H_ */
