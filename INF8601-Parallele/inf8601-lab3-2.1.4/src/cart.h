/*
 * cart.h
 *
 *  Created on: 2011-11-15
 *      Author: francis
 */

#ifndef CART_H_
#define CART_H_

#include "grid.h"

typedef struct cart2d {
	grid_t **grids;
	int width;
	int height;
	int block_x;
	int block_y;
	int **dims;
	int **pos;
} cart2d_t;

cart2d_t *make_cart2d(int width, int height, int dim_x, int dim_y);
grid_t *cart2d_get_grid(cart2d_t *cart, int x, int y);
void cart2d_grid_merge(cart2d_t *cart, grid_t *grid);
void cart2d_grid_split(cart2d_t *cart, grid_t *grid);
void cart2d_padding(cart2d_t *cart, int padding);
void free_cart2d(cart2d_t *cart);
int **decomp2d(int width, int height, int block_x, int block_y);
int **decomp2d_pos(int **dims, int block_x, int block_y);
void free_decomp2d(int **dims);

#endif /* CART_H_ */
