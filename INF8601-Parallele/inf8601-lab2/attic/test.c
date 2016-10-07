/*
 * test.c
 *
 *  Created on: 2011-10-07
 *      Author: francis
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "stripe.h"
#include "layer.h"
#include "list.h"

float tol = 0.0001;

void test_stripe_array_1()
{
	printf("test_stripe_array_1\n");
	int len = 10;
	stripe_array_t *a = make_stripe_array(len);
	free_stripe_array(a);
}

void test_stripe_array_2()
{
	printf("test_stripe_array_2\n");
	int len = 10;
	stripe_array_t *a = make_stripe_array(len);
	a->stripes[0] = make_stripe(100, 100);
	free_stripe_array(a);
}

void test_stripe_split_1()
{
	printf("test_stripe_split_1\n");
	stripe_t *s1 = make_stripe(5,10);
	stripe_t *s2 = make_stripe(5,10);
	stripe_set_all(s1, 0.0);
	stripe_set_inner(s1, 1.0);
	stripe_set_bounds(s1);
	stripe_set_all(s2, 1.0);
	float diff = stripe_diff(s1, s2);
	assert(diff < tol);
	free_stripe(s1);
	free_stripe(s2);
}

void test_stripe_split_2()
{
	printf("test_stripe_split_2\n");
	int w1 = 5;
	int h1 = 10;
	int w2 = 0;
	int h2 = 0;
	stripe_t *s1 = make_stripe(5,10);
	stripe_set_all(s1, 0.0);
	stripe_set_inc(s1, 0.0);
	stripe_set_bounds(s1);
	stripe_array_t *a = NULL;
	stripe_split(s1, &a, 3);
	stripe_array_trimmed_size(a, &w2, &h2);
	stripe_t *s2 = NULL;
	stripe_merge(a, &s2);
	stripe_set_bounds(s2);

	float diff = stripe_diff(s1, s2);
	assert(diff < tol);
	free_stripe(s1);
	free_stripe(s2);
	free_stripe_array(a);
}

void test_stripe_xchg_bounds_1()
{
	int i, w = 5, h = 10;
	stripe_t *s1 = make_stripe(w,h);
	stripe_t *s2 = make_stripe(w,h);
	stripe_set_all(s1, 0.0);
	stripe_set_all(s2, 1.0);
	stripe_xchg_bounds(s1, s2);
/*
	dump_stripe(s1);
	printf("\n");
	dump_stripe(s2);
*/
	free_stripe(s1);
	free_stripe(s2);
}

void test_layer_1()
{
	printf("test_layer_1\n");
	int len = 5;
	int width = 5;
	int height = 10;
	layers_t *l = make_layers(len);
	alloc_layers(l, 5, 10);
	reset_layers(l, 1.0);
	//dump_layers(l);
	free_layers(l);
}

void test_load_layer()
{
	printf("test_load_layer\n");
	layers_t *l = NULL;
	init_layers("tests/center-red.png", &l);
	//dump_layers(l);
	free_layers(l);
}

void test_split_layers()
{
	printf("test_split_layer\n");
	int i;
	layers_t *l = make_layers(2);
	alloc_layers(l, 5, 10);
	for(i=0; i < l->len; i++) {
		stripe_set_inc(l->layers[i]->stripes[0], 0.0);
	}
	dump_layers(l);
	split_layers(l, 2);
	dump_layers(l);
	merge_layers(l);
	dump_layers(l);
	free_layers(l);
}

void test_foreach()
{
	stripe_array_t *a = make_stripe_array(3);
	alloc_stripes(a, 5, 10);
	foreach_stripe_1(a, stripe_set_inc, 0.0);
	//foreach_stripe_0(a, dump_stripe);
	free_stripe_array(a);
}

int main(int argc, char **argv)
{
	test_stripe_array_1();
	test_stripe_array_2();
	test_stripe_split_1();
	test_stripe_split_2();
	test_stripe_xchg_bounds_1();
	test_layer_1();
	test_load_layer();
	test_split_layers();
	test_foreach();
	return 0;
}

