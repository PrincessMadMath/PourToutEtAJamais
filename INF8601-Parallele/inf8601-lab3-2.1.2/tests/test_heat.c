/*
 * test_heat.c
 *
 *  Created on: 2011-11-17
 *      Author: francis
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "grid.h"
#include "heat.h"
#include "unittest.h"

int verbose = 0;

void test_heat_diffuse()
{
	double exp, act;
	grid_t *g0 = make_grid(10, 10, 1);
	grid_t *g1 = make_grid(10, 10, 1);
	grid_t *g2 = make_grid(10, 10, 1);
	g0->dbl[IX2(5,5,12)] = 100.0;

	grid_set_min(g0, g1);
	grid_set_min(g0, g2);

	if (verbose) {
		printf("before\n");
		printf("g1\n");
		dump_grid(g1);
		printf("g2\n");
		dump_grid(g2);
	}

	heat_diffuse(g1, g2);

	if (verbose) {
		printf("after\n");
		printf("g1\n");
		dump_grid(g1);
		printf("g2\n");
		dump_grid(g2);
	}

	SWAP(g1, g2);
	grid_set_min(g0, g1);
	grid_set_min(g0, g2);
	heat_diffuse(g1, g2);

	if (verbose) {
		printf("after\n");
		printf("g1\n");
		dump_grid(g1);
		printf("g2\n");
		dump_grid(g2);
	}

	exp = 200.0;
	grid_sum(g2, &act);

	if (verbose)
		printf("exp=%f act=%f\n", exp, act);
	assert_range(exp, act, "test_heat_diffuse_1");

	free_grid(g1);
	free_grid(g2);
}

int main(int argc, char **argv)
{
	test_heat_diffuse();
	return 0;
}
