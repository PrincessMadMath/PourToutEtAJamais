/*
 * grid.h
 *
 *  Created on: 2011-11-15
 *      Author: francis
 */

#ifndef GRID_H_
#define GRID_H_

#include <stdio.h>
#include <stdlib.h>

#define IX2(i, j, w) ((i)+((j)*(w)))

typedef struct grid {
	double *dbl;
	int *data;
	int width;
	int height;
	int padding;
	int pw;
	int ph;
} grid_t;

grid_t *make_grid(int width, int height, int padding);
void free_grid(grid_t *grid);
grid_t *grid_clone(grid_t *grid);
void grid_set(grid_t *grid, double value);
void grid_set_inner(grid_t *grid, double value);
void grid_set_increment(grid_t *grid);
void grid_set_min(grid_t *min, grid_t *dst);
void grid_set_bounds1(grid_t *grid);
void grid_sum(grid_t *grid, double *sum);
void grid_multiply(grid_t *grid, double factor);
void grid_max(grid_t *grid, double *value);
void grid_copy(grid_t *min, grid_t *dst);
void grid_copy_block(grid_t *min, int x1, int y1, int w, int h, grid_t *dst, int x2, int y2);
grid_t *grid_padding(grid_t *grid, int padding);
void dump_grid(grid_t *grid);
void fdump_grid(grid_t *grid, FILE *f);

#endif /* GRID_H_ */
