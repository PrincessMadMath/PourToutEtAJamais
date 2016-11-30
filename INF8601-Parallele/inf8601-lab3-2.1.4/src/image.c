/*
 * image.c
 *
 *  Created on: 2011-11-17
 *      Author: francis
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <png.h>
#include <limits.h>

#include "grid.h"
#include "image.h"
#include "memory.h"
#include "color.h"
#include "util.h"

image_t *make_image(int width, int height, int rowbytes) {
	int y;

	image_t *image = malloc(sizeof(struct image));
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

	done: return image;
	err: free_image(image);
	image = NULL;
	goto done;
}

void free_image(image_t *image) {
	int y;
	if (image == NULL)
		return;
	if (image->rows != NULL) {
		for (y = 0; y < image->height; y++) {
			if (image->rows[y] != NULL)
				FREE(image->rows[y]);
		}
		FREE(image->rows);
	}
	FREE(image);
	return;
}

FILE *open_png(char *path) {
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
	done: return fp;
	err: if (fp != NULL)
		fclose(fp);
	fp = NULL;
	goto done;
}

image_t *load_png(char *png) {
	int width = 0;
	int height = 0;
	FILE *f = NULL;
	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;
	png_infop end_info = NULL;
	png_uint_32 rowbytes = 0;
	image_t *image = NULL;

	f = open_png(png);
	if (f == NULL)
		goto err;

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
			(png_voidp) NULL, NULL, NULL);
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

	image = make_image(width, height, rowbytes);
	if (image == NULL)
		goto err;
	png_read_image(png_ptr, image->rows);
	png_read_end(png_ptr, end_info);

	done: if (f != NULL)
		fclose(f);
	if (png_ptr != NULL)
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
	return image;
	err: free_image(image);
	image = NULL;
	goto done;
}

int save_image_ppm(char *path, struct rgb *image, int width, int height) {
	FILE *f = NULL;

	if (image == NULL || path == NULL)
		return -1;

	if ((f = fopen(path, "wb+")) == NULL) {
		perror("Failed to open output file");
		return -1;
	}

	fprintf(f, "P6\n%d %d\n%d\n", width, height, 255);
	fwrite(image, sizeof(struct rgb), width * height, f);
	fclose(f);
	return 0;
}

int save_image_uchar(char *path, unsigned char *image, int width, int height) {
	FILE *f = NULL;

	if (image == NULL || path == NULL)
		return -1;

	if ((f = fopen(path, "wb")) == NULL) {
		perror("Failed to open output file");
		return -1;
	}

	fprintf(f, "P6\n%d %d\n%d\n", width, height, 255);
	fwrite(image, sizeof(unsigned char), width * height * 3, f);
	fclose(f);
	return 0;
}

/*
 * Source: http://zarb.org/~gc/html/libpng.html
 */
int save_image_png(image_t *image, char* file_name) {
	int ret = 0;
	if (image == NULL || file_name == NULL)
		goto error;
	int width = image->width;
	int height = image->height;
	/* create file */
	FILE *fp = fopen(file_name, "wb");
	ERR_ASSERT(fp, "Failed to open output file");

	/* initialize stuff */
	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
			NULL, NULL, NULL);
	ERR_ASSERT(png_ptr, "[write_png_file] png_create_write_struct failed");

	png_infop info_ptr = png_create_info_struct(png_ptr);
	ERR_ASSERT(info_ptr, "[write_png_file] png_create_info_struct failed");

	ERR_ASSERT(!setjmp(png_jmpbuf(png_ptr)),
			"[write_png_file] Error during init_io");

	png_init_io(png_ptr, fp);

	/* write header */
	ERR_ASSERT(!setjmp(png_jmpbuf(png_ptr)),
			"[write_png_file] Error during writing header");

	png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_RGB,
			PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE,
			PNG_FILTER_TYPE_BASE);

	png_write_info(png_ptr, info_ptr);

	/* write bytes */
	ERR_ASSERT(!setjmp(png_jmpbuf(png_ptr)),
			"[write_png_file] Error during writing bytes");

	png_write_image(png_ptr, image->rows);

	/* end write */
	ERR_ASSERT(!setjmp(png_jmpbuf(png_ptr)),
			"[write_png_file] Error during end of write");

	png_write_end(png_ptr, NULL);

	fclose(fp);
	done: return ret;
	error: ret = -1;
	goto done;
}

void dump_image(image_t *image, int chan) {
	if (image == NULL)
		return;

	int i, j;
	int width = image->width;
	int height = image->height;

	printf("width=%d height=%d\n", width, height);
	for (j = 0; j < height; j++) {
		png_byte *row = image->rows[j];
		for (i = 0; i < width; i++) {
			png_byte *pix = &(row[i * 4]);
			printf("%3d ", pix[chan]);
		}
		printf("\n");
	}
}

grid_t *grid_from_image(image_t *image, int chan) {
	grid_t *grid = NULL;
	if (image == NULL)
		goto err;
	if (chan > CHAN_ALPHA)
		goto err;

	int i, j;
	int width, height;

	width = image->width;
	height = image->height;

	grid = make_grid(width, height, 0);
	if (grid == NULL)
		goto err;

	int div = UCHAR_MAX * UCHAR_MAX;
	for (j = 0; j < height; j++) {
		png_byte *row = image->rows[j];
		for (i = 0; i < width; i++) {
			png_byte *pix = &(row[i * 4]);
			int index = j * width + i;
			grid->dbl[index] = ((float) (pix[chan] * pix[CHAN_ALPHA])) / div;
		}
	}

	done: return grid;
	err:
	FREE(grid);
	grid = NULL;
	goto done;
}

image_t *grid_to_image(grid_t *grid) {
	if (grid == NULL)
		return NULL;

	double max;
	int i, j;
	int w = grid->pw;
	int h = grid->ph;
	image_t	*img = make_image(grid->width, grid->height, 3 * w);
	ERR_NOMEM(img);

	/* convert double values to char */
	grid_max(grid, &max);

	int interval = get_color_interval((float) max);
	float interval_inv = get_color_interval_inv((float) max);

	for (j = 0; j < h; j++) {
		png_byte *row = img->rows[j];
		for (i = 0; i < w; i++) {
			int index = IX2(i,j,w);
			int offset = i * 3;
			struct rgb c;
			value_color(&c, grid->dbl[index], interval, interval_inv);
			row[offset + 0] = c.r;
			row[offset + 1] = c.g;
			row[offset + 2] = c.b;
		}
	}
error:
	return img;
}

int save_grid_png(grid_t *grid, char *output) {
	if (grid == NULL || output == NULL)
		goto error;

	image_t *img = grid_to_image(grid);
	ERR_NOMEM(img);

	return save_image_png(img, output);
error:
	return -1;
}
