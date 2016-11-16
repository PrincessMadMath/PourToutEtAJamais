/*
 * heat.h
 *
 *  Created on: 2011-11-15
 *      Author: francis
 */

#ifndef HEAT_H_
#define HEAT_H_

#include "grid.h"

#define SWAP(p1, p2) typeof(p1) tmp = p1; p1 = p2; p2 = tmp;

void heat_diffuse(grid_t *curr, grid_t *next);

#endif /* HEAT_H_ */
