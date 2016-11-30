/*
 * test_cart.c
 *
 *  Created on: 2011-11-16
 *      Author: francis
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "grid.h"
#include "cart.h"
#include "heat.h"
#include "unittest.h"

void test_decomp2d() {
	int width = 99;
	int height = 99;
	int x = 2;
	int y = 5;

	int **dims = decomp2d(width, height, x, y);
	assert_equals(50, dims[0][0], "test_decomp2d_00");
	assert_equals(49, dims[0][1], "test_decomp2d_01");
	assert_equals(20, dims[1][0], "test_decomp2d_10");
	assert_equals(20, dims[1][1], "test_decomp2d_11");
	assert_equals(20, dims[1][2], "test_decomp2d_12");
	assert_equals(20, dims[1][3], "test_decomp2d_13");
	assert_equals(19, dims[1][4], "test_decomp2d_14");
	free_decomp2d(dims);
}

void test_make_cart2d() {
	int width = 99;
	int height = 99;
	int x = 2;
	int y = 5;
	cart2d_t *cart = make_cart2d(width, height, x, y);
	grid_t *g00 = cart2d_get_grid(cart, 0, 0);
	assert_equals(g00->width, 50, "test_mark_cart2d_00x");
	assert_equals(g00->height, 20, "test_mark_cart2d_00y");
	grid_t *g04 = cart2d_get_grid(cart, 0, 4);
	assert_equals(g04->width, 50, "test_mark_cart2d_04x");
	assert_equals(g04->height, 19, "test_mark_cart2d_04y");
	grid_t *g10 = cart2d_get_grid(cart, 1, 0);
	assert_equals(g10->width, 49, "test_mark_cart2d_10x");
	assert_equals(g10->height, 20, "test_mark_cart2d_10y");
	grid_t *g14 = cart2d_get_grid(cart, 1, 4);
	assert_equals(g14->width, 49, "test_mark_cart2d_14x");
	assert_equals(g14->height, 19, "test_mark_cart2d_14y");
	free_cart2d(cart);
}

void test_cart2d_split_merge() {
	int width = 20;
	int height = 20;
	int x = 2;
	int y = 5;
	double exp, act;
	grid_t *grid = make_grid(width, height, 0);
	grid_set_increment(grid);
	grid_sum(grid, &exp);
	cart2d_t *cart = make_cart2d(width, height, x, y);
	cart2d_grid_split(cart, grid);
	/*
	 int i, j;
	 printf("main grid\n");
	 dump_grid(grid);
	 for (j=0; j<y; j++) {
	 for (i=0; i<x; i++) {
	 printf("grid (%d,%d)\n", i, j);
	 dump_grid(cart->grids[IX2(i,j,x)]);
	 }
	 }
	 */
	grid_set(grid, 0.0);
	cart2d_grid_merge(cart, grid);
	grid_sum(grid, &act);
	//dump_grid(grid);
	assert_range(exp, act, "test_split_merge");

	free_cart2d(cart);
	free_grid(grid);
}

void test_cart2d_heat() {
	int d = 10;
	int x = 2;
	int y = 2;
	int i, j;

	grid_t *g0 = make_grid(d, d, 0);
	grid_t *g1 = make_grid(d, d, 0);
	grid_t *g2 = make_grid(d, d, 0);
	g0->dbl[IX2(5,5,12)] = 100.0;
	grid_set_min(g0, g1);
	grid_set_min(g0, g2);

	cart2d_t *cart0 = make_cart2d(d, d, x, y);
	cart2d_t *cart1 = make_cart2d(d, d, x, y);
	cart2d_t *cart2 = make_cart2d(d, d, x, y);
	cart2d_grid_split(cart0, g0);
	cart2d_grid_split(cart1, g1);
	cart2d_grid_split(cart2, g2);
	cart2d_padding(cart0, 1);
	cart2d_padding(cart1, 1);
	cart2d_padding(cart2, 1);
	for (j = 0; j < y; j++) {
	for (i = 0; i < x; i++) {
		grid_t *gg0 = cart2d_get_grid(cart0, i, j);
		grid_t *gg1 = cart2d_get_grid(cart1, i, j);
		grid_t *gg2 = cart2d_get_grid(cart2, i, j);
		grid_set_min(gg0, gg1);
		heat_diffuse(gg1, gg2);
		//printf("grid (%d,%d)\n", i, j);
		//dump_grid(gg2);
	}
	}
}

int main(int argc, char **argv) {
	test_decomp2d();
	test_make_cart2d();
	test_cart2d_split_merge();
	test_cart2d_heat();
	return 0;
}
