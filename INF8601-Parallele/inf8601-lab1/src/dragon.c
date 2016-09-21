/*
 * dragon.c
 *
 *	Created on: 2011-06-26
 *	Authors:	Francis Giraldeau
 *			Pier-Luc St-Onge
 *
 * http://en.wikipedia.org/wiki/Dragon_curve
 */

#define _GNU_SOURCE
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "dragon.h"
#include "color.h"

xy_t compute_position(int64_t i)
{
	xy_t position;
	position.x = 0;
	position.y = 0;
	if (i > 0) {
		int64_t mask = 1;
		int64_t position_y = 1;
		position.x = 1;
		position.y = 1;
		while ((i ^ mask) > mask)  {
			mask <<= 1;
			position_y -= position.x;
			position.x += position.y;
			position.y  = position_y;
		}
		if (i ^ mask) {
			xy_t delta = compute_position((mask << 1) - i);
			position_y -= position.x - delta.x;
			position.x += position.y - delta.y;
			position.y = position_y;
		}
	}
	return position;
}

xy_t compute_orientation(int64_t i)
{
	xy_t orientation;
	orientation.x = 1;
	orientation.y = 1;
	if (i > 0) {
		int64_t mask = 1;
		while ((i ^ mask) > mask) { mask <<= 1; }
		orientation = compute_orientation((mask << 1) - (i + 1));
		rotate_right(&orientation);
	}
	return orientation;
}

/* draw dragon in raw matrix */
int dragon_draw_raw(uint64_t start, uint64_t end, char *dragon, int width, int height, limits_t limits, char id)
{
	//printf("start=%" PRId64" end=%"PRId64" id=%d\n", start, end, id);
	if (end < start)
		printf("error: start=%"PRId64" > end=%"PRId64"\n", start, end);

	if (end == start)
		return 0;

	xy_t position;
	xy_t orientation;
	int i, j;
	uint64_t n;
	position = compute_position(start);
	orientation = compute_orientation(start);

	// draw dragon
	position.x -= limits.minimums.x;
	position.y -= limits.minimums.y;
	int area = width * height;
	for (n = start + 1; n <= end; n++) {
		j = (position.x + (position.x + orientation.x)) >> 1;
		i = (position.y + (position.y + orientation.y)) >> 1;
		int index = i * width + j;
		if (index < 0 || index > area) {
			printf("index is out of range\n");
			return -1;
		}
		dragon[index] = id;
		position.x += orientation.x;
		position.y += orientation.y;
		if (((n & -n) << 1) & n)
			rotate_left(&orientation);
		else
			rotate_right(&orientation);
	}
	return 0;
}

void init_canvas(int start, int end, char *canvas, char value)
{
    int i;
    for (i = start; i < end; i++) {
        canvas[i] = value;
    }
}

void dump_canvas(char *canvas, int width, int height)
{
	int i, j;

	printf("width=%d height=%d\n", width, height);
	for (i = 0; i < width; i++) {
		for (j = 0; j < height; j++) {
			printf("%d ", canvas[j * width + i]);
		}
		printf("\n");
	}
}

void dump_canvas_rgb(struct rgb *canvas, int width, int height)
{
	int i, j;

	printf("width=%d height=%d\n", width, height);
	for (i = 0; i < width; i++) {
		for (j = 0; j < height; j++) {
			struct rgb *pix = &canvas[j * width + i];
			printf("%d %d %d ", pix->r, pix->g, pix->b);
		}
		printf("\n");
	}
}

void scale_dragon(int start, int end, struct rgb *image, int image_width, int image_height,
        char *dragon, int dragon_width, int dragon_height, struct palette *palette)
{
    int i, j, x, y;
    int scale_x = dragon_width / image_width + 1;
    int scale_y = dragon_height / image_height + 1;
    int scale = (scale_x > scale_y ? scale_x : scale_y);
    int deltaJ = (scale * image_width - dragon_width) / 2;
    int deltaI = (scale * image_height - dragon_height) / 2;
    struct rgb *colors = palette->colors;

    for (y = start; y < end; y++) {
        int i1 = y * scale - deltaI;
        int i2 = i1 + scale;
        if (i1 < 0) i1 = 0;
        if (i2 > dragon_height) i2 = dragon_height;
        for (x = 0; x < image_width; x++) {
            int j1 = x * scale - deltaJ, j2 = j1 + scale;
            int red = 0;
            int green = 0;
            int blue = 0;
            int cnt = 0;
            if (j1 < 0) j1 = 0;
            if (j2 > dragon_width) j2 = dragon_width;
            for (i = i1; i < i2; i++) {
                for (j = j1; j < j2; j++) {
                    int id = dragon[i * dragon_width + j];
                    if (id >= 0) {
                        red     += colors[id].r;
                        green   += colors[id].g;
                        blue    += colors[id].b;
                    } else {
                        red     += 255;
                        green   += 255;
                        blue    += 255;
                    }
                    cnt++;
                }
            }
            int index = y * image_width + x;
            if (cnt == 0) {
                image[index] = white;
            } else {
                image[index].r = (unsigned char) (red   / cnt);
                image[index].g = (unsigned char) (green / cnt);
                image[index].b = (unsigned char) (blue  / cnt);
            }
        }
    }
}

int dragon_draw_serial(char **canvas, struct rgb *image, int width, int height, uint64_t size, int nb_colors)
{
	int ret = 0;
	char *dragon = NULL;
	struct palette *palette = NULL;
	limits_t limits;

	if (dragon_limits_serial(&limits, size, 0) < 0)
		goto err;

	int dragon_width = limits.maximums.x - limits.minimums.x;
	int dragon_height = limits.maximums.y - limits.minimums.y;
	int area = dragon_width * dragon_height;
	int m;

	dragon = (char*)malloc(sizeof(char) * area);
	if (dragon == NULL)
		goto err;

	palette = init_palette(nb_colors);
	if (palette == NULL)
		goto err;

	// clear dragon
	init_canvas(0, area, dragon, -1);

	// Draw dragon
	for (m = 0; m < nb_colors; m++) {
		uint64_t start = m * size / nb_colors;
		uint64_t end = (m + 1) * size / nb_colors;
		dragon_draw_raw(start, end, dragon, dragon_width, dragon_height, limits, m);
	}

	// Scale dragon to fit the final image
	scale_dragon(0, height, image, width, height, dragon, dragon_width, dragon_height, palette);

done:
	free_palette(palette);
	*canvas = dragon;
	return ret;

err:
	FREE(dragon);
	ret = -1;
	goto done;
}

int write_img(struct rgb *image, char *file, int width, int height)
{
	FILE *f = NULL;

	if (image == NULL)
		return -1;

	if (file) {
		if ((f = fopen(file, "wb")) == NULL) {
			char *msg;
			if (asprintf(&msg, "Failed to open %s", file) < 0)
				perror("Failed to open output file");
			else
				perror(msg);
			return -1;
		}
	}

	fprintf(f, "P6\n%d %d\n%d\n", width, height, 255);
	fwrite(image, sizeof(struct rgb), width * height, f);
	fclose(f);
	return 0;
}

void dump_limits(limits_t *limits)
{
	if (limits == NULL)
		return;
	printf("(%"PRId64",%"PRId64";%"PRId64",%"PRId64")\n",
		limits->minimums.x,
		limits->minimums.y,
		limits->maximums.x,
		limits->maximums.y);
}

int cmp_limits(limits_t *l1, limits_t *l2)
{
	if (l1 == NULL || l2 == NULL)
		return -1;
	return !(l1->maximums.x == l2->maximums.x &&
		l1->maximums.y == l2->maximums.y &&
		l1->minimums.x == l2->minimums.x &&
		l1->minimums.y == l2->minimums.y);
}
/*
 * compare each position exp(i,j) with act(i,j)
 * return the number of pixels that doesn't match
 */
int cmp_canvas(char *exp, char *act, int width, int height, int verbose)
{
	int i, j;
	int sum = 0;
	int index;
	if (exp == NULL || act == NULL)
		return -1;
	#pragma omp parallel for reduction(+:sum) private(index, j)
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			index = i * width + j;
			if (exp[index] != act[index]) {
				if (verbose)
					printf("pix error (%5d, %5d) expected=%2d actual=%2d\n", j, i, exp[index], act[index]);
				sum += 1;
			}
		}
	}
	return sum;
}

void piece_init(piece_t *piece)
{
	if (piece == NULL)
		return;
	piece->position.x = 0;
	piece->position.y = 0;
	piece->orientation.x = 1;
	piece->orientation.y = 1;
	piece->limits.minimums = piece->position;
	piece->limits.maximums = piece->position;
}

int dragon_limits_serial(limits_t *lim, uint64_t nbIterations, __attribute__((unused)) int nb_thread)
{
	piece_t piece;
	piece_init(&piece);
	uint64_t start = 0;
	piece_limit(start, nbIterations, &piece);
	*lim = piece.limits;
	return 0;
}

struct rgb *make_canvas(int width, int height)
{
	int area;
	area = height * width;
	if (area <= 0) {
		return NULL;
	}
	return (struct rgb *) malloc(sizeof(struct rgb) * area);
}

void piece_limit(int64_t start, int64_t end, piece_t *m)
{
	int64_t n;
	xy_t *position = &m->position;
	xy_t *orientation = &m->orientation;
	xy_t *minimums = &m->limits.minimums;
	xy_t *maximums = &m->limits.maximums;
	for (n = start + 1; n <= end; n++) {
		position->x += orientation->x;
		position->y += orientation->y;

		if (((n & -n) << 1) & n)
			rotate_left(orientation);
		else
			rotate_right(orientation);
		if (minimums->x > position->x) minimums->x = position->x;
		if (minimums->y > position->y) minimums->y = position->y;
		if (maximums->x < position->x) maximums->x = position->x;
		if (maximums->y < position->y) maximums->y = position->y;
	}
}
/*
 * merge m2 into m1
 * This operation is associative, but not commutative
 */
void piece_merge(piece_t *m1, piece_t m2)
{
	xy_t orientation;

	// Initiale orientation of m2 (1,1)
	xy_t *min1 = &m1->limits.minimums;
	xy_t *max1 = &m1->limits.maximums;
	xy_t *min2 = &m2.limits.minimums;
	xy_t *max2 = &m2.limits.maximums;

	orientation.x = 1;
	orientation.y = 1;

	// Rotate piece #2
	while ( orientation.x != m1->orientation.x ||
			orientation.y != m1->orientation.y)
	{
		rotate_left(&m2.position);
		rotate_left(&m2.orientation);
		limits_invert(&m2.limits);
		rotate_left(&orientation);
	}

	// m2 limits according to m1
	min2->x += m1->position.x;
	min2->y += m1->position.y;
	max2->x += m1->position.x;
	max2->y += m1->position.y;

	// update last segment of m1
	m1->position.x += m2.position.x;
	m1->position.y += m2.position.y;
	m1->orientation = m2.orientation;

	// update m1 limits
	if (min1->x > min2->x) min1->x = min2->x;
	if (min1->y > min2->y) min1->y = min2->y;
	if (max1->x < max2->x) max1->x = max2->x;
	if (max1->y < max2->y) max1->y = max2->y;

}
void rotate_left(xy_t *xy)
{
	int64_t tmp_y = xy->x;
	xy->x = -xy->y;
	xy->y = tmp_y;
}

void rotate_right(xy_t *xy)
{
	int64_t tmp_y = -xy->x;
	xy->x = xy->y;
	xy->y = tmp_y;
}

void limits_invert(limits_t *limites)
{
	int64_t nouveauMaxY = limites->maximums.x;
	limites->maximums.x = -limites->minimums.y;
	limites->minimums.y = limites->minimums.x;
	limites->minimums.x = -limites->maximums.y;
	limites->maximums.y = nouveauMaxY;
}
