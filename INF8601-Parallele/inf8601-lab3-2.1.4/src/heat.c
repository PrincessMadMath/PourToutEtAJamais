/*
 * heat.c
 *
 *  Created on: 2011-11-15
 *      Author: francis
 */

#include <stdlib.h>
#include <stdio.h>

#include "heat.h"
#include "grid.h"

void heat_diffuse(grid_t *curr, grid_t *next) {
	if (curr == NULL || next == NULL)
		return;
	if (curr->pw != next->pw || curr->ph != next->ph)
		return;
	grid_t g1 = *curr;
	grid_t g2 = *next;
	int i, j;
	int w = g1.pw;
	int h = g1.ph;
	for (j = 1; j < h - 1; j++) {
		for (i = 1; i < w - 1; i++) {
			g2.dbl[IX2(i,j,w)] =
					g1.dbl[IX2(i,j,w)] + 0.25 * (
					g1.dbl[IX2(i-1,j,w)] +
					g1.dbl[IX2(i+1,j,w)] +
					g1.dbl[IX2(i,j-1,w)] +
					g1.dbl[IX2(i,j+1,w)] - 4 * g1.dbl[IX2(i,j,w)]
					);
		}
	}
	grid_set_bounds1(next);
}
