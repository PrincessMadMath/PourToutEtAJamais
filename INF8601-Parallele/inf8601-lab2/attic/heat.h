/*
 * heat.h
 *
 *  Created on: 2011-10-10
 *      Author: francis
 */

#ifndef HEAT_H_
#define HEAT_H_

#define SWAP(p1, p2) typeof(p1) tmp = p1; p1 = p2; p2 = tmp;

typedef struct params params_t;

struct params {
	int iter;
	float max_heat;
	float max_diff;
	float max_sink;
};

#endif /* HEAT_H_ */
