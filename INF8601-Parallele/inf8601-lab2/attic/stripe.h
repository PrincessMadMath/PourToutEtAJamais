/*
 * stripe.h
 *
 *  Created on: 2011-10-07
 *      Author: francis
 */

#ifndef STRIPE_H_
#define STRIPE_H_

#define IX(i, j, w) ((i)+((j)*(w)))
typedef struct stripe stripe_t;
typedef struct stripe_array stripe_array_t;

struct stripe {
	float *data;
	int padding;
	int width;
	int height;
};

struct stripe_array {
	stripe_t **stripes;
	int len;
};

stripe_t *make_stripe(int width, int height);
void free_stripe(stripe_t *stripe);
stripe_array_t *make_stripe_array(int len);
void free_stripe_array(stripe_array_t *array);

int alloc_stripes(stripe_array_t *a, int width, int height);
int stripe_split(stripe_t *stripe, stripe_array_t **array, int nb);
int stripe_merge(stripe_array_t *array, stripe_t **stripe);
void stripe_set_all(stripe_t *stripe, float value);
void dump_stripe(stripe_t *stripe);
void stripe_set_inner(stripe_t *stripe, float value);
void stripe_set_bounds(stripe_t *stripe);
void stripe_set_inc(stripe_t *stripe, float start);
void stripe_mul(stripe_t *stripe, float factor);
void stripe_array_trimmed_size(stripe_array_t *array, int *w, int *h);
float stripe_diff(stripe_t *s1, stripe_t *s2);
void stripe_xchg_bounds(stripe_t *s1, stripe_t *s2);
void foreach_stripe_0(stripe_array_t *s, void(*func)(stripe_t *));
void foreach_stripe_1(stripe_array_t *s, void(*func)(stripe_t *, float), float f);
float stripe_sum_inner(stripe_t *a);
float stripe_array_sum_inner(stripe_array_t *a);

#endif /* STRIPE_H_ */
