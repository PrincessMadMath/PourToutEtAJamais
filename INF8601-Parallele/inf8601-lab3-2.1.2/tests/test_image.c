/*
 * test_image.c
 *
 *  Created on: 2011-11-17
 *      Author: francis
 */


#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "image.h"
#include "grid.h"
#include "heat.h"
#include "cart.h"
#include "unittest.h"

void test_image_load()
{
	double exp, act;
	image_t *img = load_png("simple.png");
	//dump_image(img, CHAN_RED);
	grid_t *g1 = grid_from_image(img, CHAN_RED);
	grid_sum(g1, &exp);
	//dump_grid(g1);
	grid_multiply(g1, 100.0);
	//dump_grid(g1);
	grid_sum(g1, &act);
	assert_range(exp * 100, act, "test_image_load");
	free_image(img);
	free_grid(g1);
}

void test_grid_save_png()
{
	image_t *img = load_png("simple.png");
	grid_t *g1 = grid_from_image(img, CHAN_RED);
	grid_multiply(g1, 100.0);
	save_grid_png(g1, "test_simple.png");
	free_grid(g1);
	free_image(img);
}

void test_image_heat_diffusion()
{
	image_t *img = load_png("simple.png");
	grid_t *g1 = grid_from_image(img, CHAN_RED);
	grid_multiply(g1, 100.0);
	grid_t *g2 = grid_clone(g1);

	heat_diffuse(g1, g2);
	save_grid_png(g2, "test_diffuse.png");

	free_grid(g1);
	free_grid(g2);
	free_image(img);
}

void test_heat_cart2d()
{
	double exp, act;
	int i;
	image_t *img = load_png("simple.png");
	grid_t *g1 = grid_from_image(img, CHAN_RED);
	grid_multiply(g1, 100.0);
	grid_t *g_exp = grid_clone(g1);
	grid_t *g_act = grid_clone(g1);

	cart2d_t *cart1 = make_cart2d(g1->width, g1->height, 2, 2);
	cart2d_grid_split(cart1, g1);
	cart2d_padding(cart1, 1);

	cart2d_t *cart2 = make_cart2d(g1->width, g1->height, 2, 2);
	cart2d_grid_split(cart2, g1);
	cart2d_padding(cart2, 1);

	for(i=0; i<4; i++) {
		grid_set_bounds1(cart1->grids[i]);
		grid_set_bounds1(cart2->grids[i]);
		heat_diffuse(cart1->grids[i], cart2->grids[i]);
	}

	cart2d_padding(cart2, 0);
	cart2d_grid_merge(cart2, g_act);

	grid_t *g_exp2 = grid_padding(g_exp, 1);
	grid_t *g_exp3 = grid_padding(g_exp, 1);

	heat_diffuse(g_exp2, g_exp3);
	grid_t *g_exp4 = grid_padding(g_exp3, 0);

	save_grid_png(g_exp4, "test_heat_cart2d_exp.png");
	save_grid_png(g_act, "test_heat_cart2d_act.png");

	grid_sum(g_exp4, &exp);
	grid_sum(g_act, &act);
	assert_range(exp, act, "test_heat_cart2d");

	free_cart2d(cart1);
	free_cart2d(cart2);
	free_image(img);
	free_grid(g1);
	free_grid(g_act);
	free_grid(g_exp);
	free_grid(g_exp2);
	free_grid(g_exp3);
	free_grid(g_exp4);
}

int main(int argc, char **argv)
{
	test_image_load();
	test_grid_save_png();
	test_image_heat_diffusion();
	test_heat_cart2d();
	return 0;
}
