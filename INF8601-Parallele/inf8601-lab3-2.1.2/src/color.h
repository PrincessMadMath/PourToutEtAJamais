/*
 * color.h
 *
 *  Created on: 2011-08-27
 *      Author: francis
 */

#ifndef COLOR_H_
#define COLOR_H_

struct rgb {
	unsigned char r;
	unsigned char g;
	unsigned char b;
};

extern const struct rgb white;
extern const struct rgb black;

int save_image(char *path, struct rgb *image, int width, int height);
int save_image_uchar(char *path, unsigned char *image, int width, int height);
void value_color(struct rgb *color, float value, int interval,
		float interval_inv);
void value_color_set_max(float max);
void hue(struct rgb **image, int width, int height);
int get_color_interval(float max);
float get_color_interval_inv(float max);
#endif /* COLOR_H_ */
