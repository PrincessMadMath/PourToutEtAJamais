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
#include <math.h>

#include "color.h"

const struct rgb white = { .r = 255, .g = 255, .b = 255 };
const struct rgb black = { .r = 0, .g = 0, .b = 0 };

/*
 * Set the maximum value for the color scale
 * The minimum color scale is 0
 * The minimum value is 4, the number of color intervals */
int get_color_interval(float max) {
	if (max < 4.0f)
		max = 4.0f;
	return (int) max / 4;
}

float get_color_interval_inv(float max) {
	if (max < 4.0f)
		max = 4.0f;
	return (float) 4 / max;
}

void value_color(struct rgb *color, float value, int interval,
		float interval_inv) {
	if (isnan(value)) {
		*color = black;
		return;
	}
	struct rgb c;
	int x = (((int) value % interval) * 255) * interval_inv;
	int i = value * interval_inv;
	switch (i) {
	case 0:
		c.r = 0;
		c.g = x;
		c.b = 255;
		break;
	case 1:
		c.r = 0;
		c.g = 255;
		c.b = 255 - x;
		break;
	case 2:
		c.r = x;
		c.g = 255;
		c.b = 0;
		break;
	case 3:
		c.r = 255;
		c.g = 255 - x;
		c.b = 0;
		break;
	case 4:
		c.r = 255;
		c.g = 0;
		c.b = x;
		break;
	default:
		c = white;
		break;
	}
	*color = c;
}

void hue(struct rgb **image, int width, int height) {
	int i, j;

	*image = (struct rgb*) calloc(width * height, sizeof(struct rgb));
	struct rgb *img = *image;
	int interval = get_color_interval((float) height);
	float interval_inv = get_color_interval_inv((float) height);
	for (j = 0; j < height; j++) {
		for (i = 0; i < width; i++) {
			value_color(&img[j * width + i], (float) j, interval,
					interval_inv);
		}
	}
}
