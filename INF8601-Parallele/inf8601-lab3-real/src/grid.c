/*
 * grid.c
 *
 *  Created on: 2011-11-15
 *      Author: francis
 */

#include <stdlib.h>
#include <stdio.h>

#include "grid.h"
#include "memory.h"

grid_t *make_grid(int width, int height, int padding) {
	grid_t *grid = (grid_t *) calloc(1, sizeof(grid_t));
	if (grid == NULL)
		return NULL;
	int count = (width + padding * 2) * (height + padding * 2);
	grid->data = (int *) calloc(count, sizeof(int));
	grid->dbl = (double *) calloc(count, sizeof(double));
	if (grid->data == NULL || grid->dbl == NULL)
		goto error;
	grid->height = height;
	grid->width = width;
	grid->padding = padding;
	grid->pw = width + padding * 2;
	grid->ph = height + padding * 2;
done:
	return grid;
error:
	free_grid(grid);
	grid = NULL;
	goto done;
}

void free_grid(grid_t *grid) {
	if (grid != NULL) {
		FREE(grid->data);
		FREE(grid->dbl);
	}
	FREE(grid);
}

grid_t *grid_clone(grid_t *grid) {
	if (grid == NULL)
		return NULL;
	grid_t *out = make_grid(grid->width, grid->height, grid->padding);
	grid_copy(grid, out);
	return out;
}

void grid_set_increment(grid_t *grid) {
	int i, j;
	if (grid == NULL)
		return;
	grid_t g = *grid;
	double value = 0.0;
	for (j = 0; j < g.ph; j++) {
		for (i = 0; i < g.pw; i++) {
			g.dbl[IX2(i,j,g.pw)] = value;
			value += 1.0;
		}
	}
}

void grid_set(grid_t *grid, double value) {
	int i, j;
	if (grid == NULL)
		return;
	grid_t g = *grid;
	for (j = 0; j < g.ph; j++) {
		for (i = 0; i < g.pw; i++) {
			g.dbl[IX2(i,j,g.pw)] = value;
		}
	}
}

void grid_set_inner(grid_t *grid, double value) {
	int i, j;
	if (grid == NULL)
		return;
	grid_t g = *grid;
	for (j = g.padding; j < g.height + g.padding; j++) {
		for (i = g.padding; i < g.width + g.padding; i++) {
			g.dbl[IX2(i,j,g.pw)] = value;
		}
	}
}

void grid_sum(grid_t *grid, double *value) {
	int i, j;
	if (grid == NULL || value == NULL)
		return;
	grid_t g = *grid;
	double sum = 0.0;
	for (j = 0; j < g.ph; j++) {
		for (i = 0; i < g.pw; i++) {
			sum += g.dbl[IX2(i,j,g.pw)];
		}
	}
	*value = sum;
}

void grid_max(grid_t *grid, double *value) {
	int i, j;
	if (grid == NULL || value == NULL)
		return;
	grid_t g = *grid;
	double max = 0.0;
	for (j = 0; j < g.ph; j++) {
		for (i = 0; i < g.pw; i++) {
			double val = g.dbl[IX2(i,j,g.pw)];
			if (val > max)
				max = val;
		}
	}
	*value = max;
}

void grid_multiply(grid_t *grid, double factor) {
	int i, j;
	if (grid == NULL)
		return;
	grid_t g = *grid;
	for (j = 0; j < g.ph; j++) {
		for (i = 0; i < g.pw; i++) {
			g.dbl[IX2(i,j,g.pw)] *= factor;
		}
	}
}

void dump_grid(grid_t *grid) {
	fdump_grid(grid, stdout);
}

void fdump_grid(grid_t *grid, FILE* f) {
	int i, j;
	if (grid == NULL)
		return;
	grid_t g = *grid;
	for (j = 0; j < g.ph; j++) {
		for (i = 0; i < g.pw; i++) {
			int index = IX2(i,j,g.pw);
			fprintf(f, "%#6.2f ", g.dbl[index]);
		}
		fprintf(f, "\n");
	}
}

void grid_copy(grid_t *src, grid_t *dst) {
	int i1, i2, j1, j2;
	grid_t s = *src;
	grid_t d = *dst;

	/* both grid must have the same dim */
	if (s.height != d.height || s.width != d.width) {
		printf("warning: invalid bounds\n");
		return;
	}

	for (j1 = s.padding, j2 = d.padding; j1 < s.height + s.padding && j2 < d.height + d.padding; j1++, j2++) {
		for (i1 = s.padding, i2 = d.padding; i1 < s.width + s.padding && i2 < d.width + d.padding; i1++, i2++) {
			d.dbl[IX2(i2,j2,d.pw)] = s.dbl[IX2(i1,j1,s.pw)];
		}
	}
}

void grid_copy_block(grid_t *src, int x1, int y1, int w, int h, grid_t *dst,
		int x2, int y2) {
	int i1, i2, j1, j2;
	grid_t s = *src;
	grid_t d = *dst;

	/* check bounds */
	if (y1 + h > s.ph || x1 + w > s.pw || y2 + h > d.ph || x2 + w > d.pw) {
		printf("warning: invalid bounds\n");
		return;
	}
	for (j1 = y1, j2 = y2; j1 < y1 + h && j2 < y2 + h; j1++, j2++) {
		for (i1 = x1, i2 = x2; i1 < x1 + w && i2 < x2 + w; i1++, i2++) {
			d.dbl[IX2(i2,j2,d.pw)] = s.dbl[IX2(i1,j1,s.pw)];
		}
	}
}

grid_t *grid_padding(grid_t *grid, int padding) {
	if (grid == NULL)
		return NULL;

	grid_t *new_grid = make_grid(grid->width, grid->height, padding);
	if (new_grid == NULL)
		return NULL;
	grid_copy(grid, new_grid);
	return new_grid;
}

/*
 * Re-heat the grid
 */
void grid_set_min(grid_t *min, grid_t *dst) {
	int i1, i2, j1, j2;
	grid_t s = *min;
	grid_t d = *dst;

	/* both grid must have the same dim */
	if (s.height != d.height || s.width != d.width) {
		printf("warning: invalid bounds\n");
		return;
	}

	for (j1 = s.padding, j2 = d.padding; j1 < s.height + s.padding && j2 < d.height + d.padding; j1++, j2++) {
		for (i1 = s.padding, i2 = d.padding; i1 < s.width + s.padding && i2 < d.width + d.padding; i1++, i2++) {
			int dst_index = IX2(i2,j2,d.pw);
			int src_index = IX2(i1,j1,s.pw);
			double act = d.dbl[dst_index];
			double desired = s.dbl[src_index];
			if (act < desired) {
				d.dbl[dst_index] = desired;
			}
		}
	}
}

/*
 * copy inner bounds to padding
 * assume a padding of one
 */
void grid_set_bounds1(grid_t *grid) {
	if (grid == NULL)
		return;
	grid_t g = *grid;
	int i, j;
	int w = g.pw;
	int h = g.ph;
	for (i = 1; i < w - 1; i++) {
		g.dbl[IX2(i,0,w)] = g.dbl[IX2(i,1,w)];
		g.dbl[IX2(i,h-1,w)] = g.dbl[IX2(i,h-2,w)];
	}
	for (j = 1; j < h - 1; j++) {
		g.dbl[IX2(0,j,w)] = g.dbl[IX2(1,j,w)];
		g.dbl[IX2(w-1,j,w)] = g.dbl[IX2(w-2,j,w)];
	}
	// corners
	g.dbl[IX2(0,0,w)] = g.dbl[IX2(1,1,w)];
	g.dbl[IX2(w-1,0,w)] = g.dbl[IX2(w-2,1,w)];
	g.dbl[IX2(0,h-1,w)] = g.dbl[IX2(1,h-2,w)];
	g.dbl[IX2(w-1,h-1,w)] = g.dbl[IX2(w-2,h-2,w)];
}

