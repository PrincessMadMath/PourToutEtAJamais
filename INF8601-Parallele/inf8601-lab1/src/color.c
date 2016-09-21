/*
 * color.c
 *
 *  Created on: 2011-08-27
 *      Author: francis
 *
 * Code related to color management
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "color.h"

const struct rgb white = { .r = 255, .g = 255, .b = 255 };
const struct rgb black = { .r = 0, .g = 0, .b = 0 };

void random_color(struct rgb *colour)
{
	if (colour == NULL)
		return;

	srand(time(NULL));
	colour->r = (unsigned char) rand();
	colour->g = (unsigned char) rand();
	colour->b = (unsigned char) rand();
}

struct palette *init_palette(int num)
{
	int i;

	struct palette *palette = (struct palette *) malloc(sizeof(struct palette));
	if (palette == NULL)
		return NULL;

	struct rgb *colours = (struct rgb *) malloc(sizeof(struct rgb) * num);
	if (colours == NULL)
		return NULL;

	palette->colors = colours;
	palette->len = num;

	/*
	 * color parameters
	 */
	double speed = 0.2;
	int max = 200;
	int phase_r = M_PI * 1 / 16;
	int phase_g = M_PI * 8 / 16;
	int phase_b = M_PI * 13 / 16;

	double step = M_PI * speed / num;
	for (i = 0; i < num; i++) {
		palette->colors[i].r = (unsigned char) max * fabs(sin(step * i * 1 + phase_r));
		palette->colors[i].g = (unsigned char) max * fabs(sin(step * i * 2 + phase_g));
		palette->colors[i].b = (unsigned char) max * fabs(sin(step * i * 3 + phase_b));
	}
	return palette;
}

void free_palette(struct palette *palette)
{
	if (palette == NULL)
		return;
	if (palette->colors == NULL)
		goto done;
	free(palette->colors);
done:
	free(palette);
	palette = NULL;
	return;
}

void dump_palette(struct palette *palette)
{
	int i;
	if (palette == NULL || palette->colors == NULL)
		return;
	for (i = 0; i < palette->len; i++) {
		printf("%d %d %d\n", palette->colors[i].r, palette->colors[i].g, palette->colors[i].b);
	}
}
