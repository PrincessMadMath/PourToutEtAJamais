/*
 * algo.h
 *
 *  Created on: 2011-09-25
 *      Author: francis
 */

#ifndef ALGO_H_
#define ALGO_H_

#include "chunk.h"

typedef int (*encode_fct)(struct chunk *);

struct encoder_def {
    const char *name;
    encode_fct encode_handler;
};

int encode_fast(struct chunk *chunk);
int encode_slow_a(struct chunk *chunk);
int encode_slow_b(struct chunk *chunk);
int encode_slow_c(struct chunk *chunk);
int encode_slow_d(struct chunk *chunk);
int encode_slow_e(struct chunk *chunk);
int encode_slow_f(struct chunk *chunk);

#endif /* ALGO_H_ */
