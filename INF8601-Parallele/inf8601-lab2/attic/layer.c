/*
 * layer.c
 *
 *  Created on: 2011-10-07
 *      Author: francis
 */

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <errno.h>
#include <png.h>
#include <math.h>

#include "layer.h"
#include "stripe.h"
#include "util.h"

const int NUM_LAYERS = LAYER_MAT2 + 1;

void free_layers(layers_t *layers)
{
	int i;
	if (layers == NULL)
		return;
	for (i=0; i < layers->len; i++) {
		free_stripe_array(layers->layers[i]);
	}
	FREE(layers->layers);
	FREE(layers);
}

layers_t *make_layers(int len)
{
	layers_t *l;

	l = calloc(1, sizeof(layers_t));
	if (l == NULL)
		goto err;
	l->layers = calloc(len, sizeof(stripe_array_t *));
	if (l->layers == NULL)
		goto err;
	l->len = len;
done:
	return l;
err:
	free_layers(l);
	l = NULL;
	goto done;
}

void free_png(struct png_image *image)
{
	int y;
	if (image == NULL)
		return;
	if (image->rows != NULL) {
		for(y = 0; y < image->height; y++) {
			if (image->rows[y] != NULL)
				FREE(image->rows[y]);
		}
		FREE(image->rows);
	}
	FREE(image);
	return;
}

struct png_image *make_png(int width, int height, int rowbytes)
{
	int y;

	struct png_image *image = malloc(sizeof(struct png_image));
    if (image == NULL)
    	goto err;
    image->width = width;
    image->height = height;
    image->rows = NULL;

    image->rows = (png_bytep*) malloc(sizeof(png_bytep) * height);
    if (image->rows == NULL)
    	goto err;

    for (y = 0; y < height; y++) {
		image->rows[y] = (png_byte*) malloc(rowbytes);
		if (image->rows[y] == NULL)
			goto err;
    }

done:
	return image;
err:
	free_png(image);
	image = NULL;
	goto done;
}

float sum(float *m, int width, int height)
{
	int i, j;
	float s = 0.0;
	for(i = 1; i < height - 1; i++) {
		for(j = 1; j < width - 1; j++) {
			int index = i * width + j;
			s += m[index];
		}
	}
	return s;
}

void dump_layer(float *m, int width, int height)
{
	int x, y;
	if (m == NULL)
		return;
	for(y = 0; y < height; y++){
		for(x = 0; x < width; x++) {
			int index = y * width + x;
			printf("%#6.2f ", m[index]);
		}
		printf("\n");
	}
}

FILE *open_png(char *path)
{
	int ret, is_png;
	png_byte header[HEADER_SIZE];
	FILE *fp = fopen(path, "rb");
	if (fp == NULL) {
		perror(path);
		goto err;
	}
	ret = fread(header, 1, HEADER_SIZE, fp);
	if (ret != 8) {
		printf("%s: error reading the file\n", path);
		goto err;
	}
	is_png = !png_sig_cmp(header, 0, HEADER_SIZE);
	if (!is_png) {
		printf("%s: is not a png file\n", path);
		goto err;
	}
	done:
	return fp;
	err:
	if (fp != NULL)
		fclose(fp);
	fp = NULL;
	goto done;
}

void dump_png(png_bytep *rows_ptr, int width, int height)
{
	int i, j;

	printf("width=%d height=%d\n", width, height);
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			printf("%d ", rows_ptr[i][j]);
		}
		printf("\n");
	}
}

struct png_image *load_png(char *png)
{
	int width = 0;
	int height = 0;
	int y = 0;
	FILE *f = NULL;
	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;
	png_infop end_info = NULL;
	png_uint_32 rowbytes = 0;
	struct png_image *image = NULL;

	f = open_png(png);
	if (f == NULL)
		goto err;

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
			(png_voidp)NULL, NULL, NULL);
	if (!png_ptr)
		goto err;

	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
		goto err;

	end_info = png_create_info_struct(png_ptr);
	if (!end_info)
		goto err;

    if (setjmp(png_jmpbuf(png_ptr))) {
        printf("[read_png_file] Error during read_image");
        goto err;
    }

	png_init_io(png_ptr, f);
    png_set_sig_bytes(png_ptr, HEADER_SIZE);
    png_read_info(png_ptr, info_ptr);
    rowbytes = png_get_rowbytes(png_ptr, info_ptr);
    width = png_get_image_width(png_ptr, info_ptr);
    height = png_get_image_height(png_ptr, info_ptr);

    if (png_get_color_type(png_ptr, info_ptr) != PNG_COLOR_TYPE_RGBA) {
    	printf("Error: PNG format is not RGBA\n");
    	goto err;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
		printf("[read_png_file] Error during read_image");
		goto err;
    }

    image = make_png(width, height, rowbytes);
    if (image == NULL)
    	goto err;
    png_read_image(png_ptr, image->rows);
    png_read_end(png_ptr, end_info);

done:
	if (f != NULL)
		fclose(f);
	if (png_ptr != NULL)
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
	return image;
err:
	free_png(image);
	image = NULL;
	goto done;
}

int load_mask(struct png_image *image, int chan, stripe_t *stripe)
{
	int i, j, size;
	int ret = 0;
	int width, height;
	if (image == NULL)
		goto err;
	if (chan > CHAN_ALPHA)
		goto err;
	width = image->width;
	height = image->height;
	size = width * height;
	float *mask = stripe->data;
	int div = UCHAR_MAX * UCHAR_MAX;
	for(j = 0; j < height; j++) {
		png_byte *row = image->rows[j];
		for(i = 0; i < width; i++) {
			png_byte *pix = &(row[i*4]);
			int index = j * width + i;
			mask[index] = ((float) (pix[chan] * pix[CHAN_ALPHA])) / div;
		}
	}

done:
	return ret;
err:
	ret = -1;
	goto done;
}

void mul_layer(layers_t *l, float factor)
{
	int i;
	for(i=0; i < l->len; i++) {
		stripe_array_t *a = l->layers[i];
		foreach_stripe_1(a, stripe_mul, factor);
	}
}

void reset_layers(layers_t *l, float value)
{
	int i;
	for(i=0; i < l->len; i++) {
		stripe_array_t *a = l->layers[i];
		foreach_stripe_1(a, stripe_set_all, value);
	}
}

int init_layers(char *path, layers_t **layers)
{
	int ret = 0, r = 0;
	layers_t *l = NULL;
	int width = 0, height = 0;
	struct png_image *image;
	stripe_t *s;
	image = load_png(path);
	if (image == NULL)
		goto err;
	width = image->width;
	height = image->height;
	l = make_layers(NUM_LAYERS);
	if (l == NULL)
		goto err;

	r = alloc_layers(l, width, height);
	if (r < 0)
		goto err;
	s = l->layers[LAYER_HEAT]->stripes[0];
	r = load_mask(image, CHAN_RED, s);
	if (r < 0)
		goto err;
	s = l->layers[LAYER_DIFF]->stripes[0];
	r = load_mask(image, CHAN_GREEN, s);
	if (r < 0)
		goto err;
	s = l->layers[LAYER_SINK]->stripes[0];
	r = load_mask(image, CHAN_BLUE, s);
	if (r < 0)
		goto err;

	stripe_set_all(l->layers[LAYER_MAT1]->stripes[0], 0.0);
	stripe_set_all(l->layers[LAYER_MAT2]->stripes[0], 0.0);

done:
	*layers = l;
	free_png(image);
	return ret;
err:
	ret = -1;
	free_layers(l);
	l = NULL;
	goto done;
}

int alloc_layers(layers_t *layers, int width, int height)
{
	int ret = 0;
	int i;
	if (layers == NULL)
		goto err;
	for (i=0; i < layers->len; i++) {
		 stripe_array_t *a = make_stripe_array(1);
		 if (a == NULL)
			 goto err;
		 stripe_t *s = make_stripe(width, height);
		 if (s == NULL)
			 goto err;
		 a->stripes[0] = s;
		 layers->layers[i] = a;
	}

done:
	return ret;
err:
	ret = -1;
	free_layers(layers);
	goto done;
}

void dump_layers(layers_t *l)
{
	int i, j;
	for (i=0; i<l->len; i++) {
		stripe_array_t *a = l->layers[i];
		foreach_stripe_0(a, dump_stripe);
	}
}

/* replaces old stripes with splited ones */
void split_layers(layers_t *layers, int nb)
{
	int i, j;
	if (layers == NULL)
		return;
	for(i=0; i < layers->len; i++) {
		stripe_array_t *a = layers->layers[i];
		stripe_array_t *b = NULL;
		stripe_split(a->stripes[0], &b, nb);
		foreach_stripe_0(b, stripe_set_bounds);
		layers->layers[i] = b;
		free_stripe_array(a);
	}
}

/* replaces old stripes with merged ones */
void merge_layers(layers_t *layers)
{
	int i, j;
	if (layers == NULL)
		return;
	for(i=0; i < layers->len; i++) {
		stripe_array_t *a = layers->layers[i];
		stripe_t *s = NULL;
		stripe_merge(a, &s);
		stripe_set_bounds(s);
		stripe_array_t *b = make_stripe_array(1);
		b->stripes[0] = s;
		layers->layers[i] = b;
		free_stripe_array(a);
	}
}
