/*
 * cart.c
 *
 *  Created on: 2011-11-15
 *      Author: francis
 */

#include <stdlib.h>
#include <stdio.h>

#include "memory.h"
#include "grid.h"
#include "cart.h"

cart2d_t *make_cart2d(int width, int height, int block_x, int block_y) {
	int i, j;
	cart2d_t *cart = (cart2d_t *) malloc(sizeof(cart2d_t));
	if (cart == NULL)
		return NULL;
	cart->grids = (grid_t **) malloc(sizeof(grid_t *) * block_x * block_y);
	if (cart->grids == NULL)
		goto error;
	cart->block_x = block_x;
	cart->block_y = block_y;
	cart->width = width;
	cart->height = height;

	/* 2D decomposition */
	int **dims = decomp2d(width, height, block_x, block_y);
	int **pos = decomp2d_pos(dims, block_x, block_y);
	if (dims == NULL || pos == NULL)
		goto error;
	for (j = 0; j < block_y; j++) {
		for (i = 0; i < block_x; i++) {
			cart->grids[IX2(i,j,block_x)] = make_grid(dims[0][i], dims[1][j], 0);
		}
	}

	cart->dims = dims;
	cart->pos = pos;
done:
	return cart;
error:
	free_cart2d(cart);
	cart = NULL;
	goto done;
}

void free_cart2d(cart2d_t *cart) {
	int i;
	if (cart != NULL) {
		int len = cart->block_x * cart->block_y;
		for (i = 0; i < len; i++) {
			free_grid(cart->grids[i]);
		}
		FREE(cart->grids);
		free_decomp2d(cart->dims);
		free_decomp2d(cart->pos);
	}
	FREE(cart);
}

grid_t *cart2d_get_grid(cart2d_t *cart, int x, int y) {
	if (cart == NULL || x >= cart->block_x || y >= cart->block_y)
		return NULL;
	return cart->grids[IX2(x,y,cart->block_x)];
}

void cart2d_set_grid(cart2d_t *cart, int x, int y, grid_t *grid) {
	if (cart == NULL || x >= cart->block_x || y >= cart->block_y)
		return;
	cart->grids[IX2(x,y,cart->block_x)] = grid;
}

void cart2d_grid_merge(cart2d_t *cart, grid_t *grid) {
	if (grid == NULL || cart == NULL)
		return;

	int x, y;
	grid_t *src_grid;
	cart2d_t c = *cart;

	for (y = 0; y < c.block_y; y++) {
		for (x = 0; x < c.block_x; x++) {
			src_grid = cart2d_get_grid(cart, x, y);
			grid_copy_block(src_grid,
					0,
					0,
					c.dims[0][x],
					c.dims[1][y],
					grid,
					c.pos[0][x],
					c.pos[1][y]);
		}
	}
}

void cart2d_grid_split(cart2d_t *cart, grid_t *grid) {
	if (grid == NULL || cart == NULL)
		return;

	int x, y;
	grid_t *dst_grid;
	cart2d_t c = *cart;

	for (y = 0; y < c.block_y; y++) {
		for (x = 0; x < c.block_x; x++) {
			dst_grid = cart2d_get_grid(cart, x, y);
			grid_copy_block(grid,
					c.pos[0][x],
					c.pos[1][y],
					c.dims[0][x],
					c.dims[1][y],
					dst_grid, 0, 0);
		}
	}
}

void cart2d_padding(cart2d_t *cart, int padding) {
	if (cart == NULL)
		return;
	int x, y;
	grid_t *grid;
	grid_t *new_grid;
	cart2d_t c = *cart;

	for (y = 0; y < c.block_y; y++) {
		for (x = 0; x < c.block_x; x++) {
			grid = cart2d_get_grid(cart, x, y);
			new_grid = grid_padding(grid, padding);
			cart2d_set_grid(cart, x, y, new_grid);
			free_grid(grid);
		}
	}
}

void decomp1d_fill(int *dim, int size, int block) {
	int i, c;
	int base = size / block;
	int mod = size % block;
	for (i = 0; i < block; i++) {
		c = base;
		if (mod > 0) {
			mod--;
			c++;
		}
		dim[i] = c;
	}
}

int **decomp2d(int width, int height, int block_x, int block_y) {
	int **dims = (int **) calloc(2, sizeof(int *));
	if (dims == NULL)
		return NULL;
	dims[0] = (int *) calloc(block_x, sizeof(int));
	dims[1] = (int *) calloc(block_y, sizeof(int));
	if (dims[0] == NULL || dims[1] == NULL)
		return NULL;
	decomp1d_fill(dims[0], width, block_x);
	decomp1d_fill(dims[1], height, block_y);
	return dims;
}

void free_decomp2d(int **dims) {
	if (dims != NULL) {
		FREE(dims[0]);
		FREE(dims[1]);
	}
	FREE(dims);
}

int **decomp2d_pos(int **dims, int block_x, int block_y) {
	int x, y, pos_x = 0, pos_y = 0;
	int **pos = (int **) calloc(2, sizeof(int *));
	if (pos == NULL)
		return NULL;
	pos[0] = (int *) calloc(block_x, sizeof(int));
	pos[1] = (int *) calloc(block_y, sizeof(int));
	if (pos[0] == NULL || pos[1] == NULL)
		return NULL;
	for (y = 0; y < block_y; y++) {
		pos_x = 0;
		for (x = 0; x < block_x; x++) {
			pos[0][x] = pos_x;
			pos[1][y] = pos_y;
			pos_x += dims[0][x];
		}
		pos_y += dims[1][y];
	}
	return pos;
}
