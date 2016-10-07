/*
 * stripe.c
 *
 *  Created on: 2011-10-07
 *      Author: francis
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>

#include "stripe.h"
#include "list.h"
#include "util.h"

stripe_t *make_stripe(int width, int height)
{
	stripe_t *stripe = (stripe_t *)calloc(1, sizeof(stripe_t));
	if (stripe == NULL)
		return NULL;
	stripe->data = malloc(sizeof(float) * width * height);
	if (stripe->data == NULL)
		goto err;
	stripe->width = width;
	stripe->height = height;
done:
	return stripe;
err:
	FREE(stripe->data);
	FREE(stripe);
	goto done;
}

void free_stripe(stripe_t *stripe)
{
	if (stripe == NULL)
		return;
	FREE(stripe->data);
	FREE(stripe);
}

stripe_array_t *make_stripe_array(int len)
{
	stripe_array_t *a = malloc(sizeof(stripe_array_t));
	if (a == NULL)
		return NULL;
	a->stripes = calloc(len, sizeof(stripe_t *));
	if (a->stripes == NULL) {
		FREE(a);
		return NULL;
	}
	a->len = len;
	return a;
}

int alloc_stripes(stripe_array_t *a, int width, int height)
{
	int i;
	if (a == NULL)
		return -1;
	for (i=0;i<a->len;i++) {
		stripe_t *s = make_stripe(width, height);
		if (s == NULL)
			return -1;
		a->stripes[i] = s;
	}
	return 0;
}

void free_stripe_array(stripe_array_t *array)
{
	int i, len;
	if (array == NULL)
		return;
	len = array->len;
	for(i=0; i < len; i++) {
		free_stripe(array->stripes[i]);
	}
	FREE(array->stripes);
	FREE(array);
}

/*
 * split the stripe into nb stripes and hold then in stripe_array
 * the caller is responsible to free the array
 * */
int stripe_split(stripe_t *stripe, stripe_array_t **array, int nb)
{
	int ret = 0;
	int i, j, k;
	int sj, dj, sj1, sj2, dj1, dj2, stripe_height, h, w, mod;
	float *src, *dst;
	stripe_array_t *a = NULL;

	if (stripe == NULL || array == NULL)
		return -1;
	a = make_stripe_array(nb);
	if (a == NULL)
		goto err;
	w = stripe->width;
	src = stripe->data;
	stripe_height = (stripe->height - 2) / nb;
	mod = (stripe->height - 2) % nb;
	sj1 = 1; sj2 = 1, dj1 = 1, dj2 = 1, h = 0;
	for (k=0; k < nb; k++) {
		sj1 = sj2;
		sj2 = sj2 + stripe_height;
		if (mod > 0) {
			sj2++; mod--;
		}
		dj1 = 1;
		dj2 = sj2 - sj1 + 1;
		h = dj2 + 1;
		stripe_t *s = make_stripe(stripe->width, h);
		if (s == NULL)
			goto err;
		a->stripes[k] = s;
		dst = s->data;
		/*
		printf("make_stripe k=%d sh=%d sj1=%d sj2=%d dh=%d dj1=%d dj2=%d \n",
				k, sj2 - sj1, sj1, sj2, dj2 - dj1, dj1, dj2);
		*/
		for (sj=sj1,dj=dj1; (sj < sj2) && (dj < dj2); sj++,dj++) {
			for (i=1; i < w-1; i++) {
				int sidx = IX(i,sj,w);
				int didx = IX(i,dj,w);
				dst[didx] = src[sidx];
			}
		}
	}

done:
	*array = a;
	return ret;
err:
	free_stripe_array(a);
	a = NULL;
	ret = -1;
	goto done;
}

/*
 * merge all stripes from the array in one stripe
 * the outer padding is reset to zero.
 * the caller is responsible to free the stripe
 * */
int stripe_merge(stripe_array_t *array, stripe_t **stripe)
{
	int ret = 0;
	int i, j, k;
	int sj, dj, sj1, sj2, dj1, dj2, stripe_height, h, w, mod, nb;
	float *src, *dst;
	stripe_t *dst_stripe = NULL;

	if (stripe == NULL || array == NULL)
		return -1;

	stripe_array_trimmed_size(array, &w, &h);
	dst_stripe = make_stripe(w, h);
	if (dst_stripe == NULL)
		goto err;
	dst = dst_stripe->data;
	nb = array->len;
	sj1 = 1; sj2 = 1; dj1 = 1; dj2 = 1;
	for (k=0; k < nb; k++) {
		stripe_t *src_stripe = array->stripes[k];
		if (src_stripe == NULL)
			continue;
		src = src_stripe->data;
		sj1 = 1;
		sj2 = src_stripe->height - 1;
		dj1 = dj2;
		dj2 += src_stripe->height - 2;
		//printf("merge_stripe k=%d sh=%d sj1=%d sj2=%d dh=%d dj1=%d dj2=%d \n",
		//		k, sj2 - sj1, sj1, sj2, dj2 - dj1, dj1, dj2);
		for (sj=sj1,dj=dj1; (sj < sj2) && (dj < dj2); sj++,dj++) {
			for (i=1; i < w-1; i++) {
				int sidx = IX(i,sj,w);
				int didx = IX(i,dj,w);
				dst[didx] = src[sidx];
			}
		}
	}

done:
	*stripe = dst_stripe;
	return ret;
err:
	free_stripe(dst_stripe);
	dst_stripe = NULL;
	ret = -1;
	goto done;
}

void stripe_array_trimmed_size(stripe_array_t *array, int *w, int *h)
{
	int width = 0;
	int height = 0;
	int k;
	if (array == NULL)
		goto done;
	for(k=0; k < array->len; k++) {
		stripe_t *s = array->stripes[k];
		if (s == NULL)
			continue;
		width = s->width - 2;
		height += s->height - 2;
	}
	width += 2;
	height += 2;
done:
	*w = width;
	*h = height;
}

void stripe_set_all(stripe_t *stripe, float value)
{
	int i;
	int size;
	if (stripe == NULL)
		return;
	size = stripe->width * stripe->height;
	float *data = stripe->data;
	for (i=0; i<size; i++)
		data[i] = value;
}

void stripe_set_inner(stripe_t *stripe, float value)
{
	int i, j;
	int size;
	if (stripe == NULL)
		return;
	stripe_t s = *stripe;
	for (j=1; j < s.height-1; j++) {
		for (i=1; i < s.width-1; i++) {
			s.data[IX(i,j,s.width)] = value;
		}
	}
}

void stripe_set_inc(stripe_t *stripe, float start)
{
	int i, j;
	int size;
	if (stripe == NULL)
		return;
	stripe_t s = *stripe;
	for (j=0; j < s.height; j++) {
		for (i=0; i < s.width; i++) {
			s.data[IX(i,j,s.width)] = start;
			start += 1.0;
		}
	}
}

void stripe_mul(stripe_t *stripe, float factor)
{
	int i, j;
	int size;
	if (stripe == NULL)
		return;
	stripe_t s = *stripe;
	for (j=0; j < s.height; j++) {
		for (i=0; i < s.width; i++) {
			s.data[IX(i,j,s.width)] *= factor;
		}
	}
}

float stripe_sum_inner(stripe_t *a)
{
	int i, j;
	float sum = 0.0;
	if (a == NULL)
		return sum;
	int w = a->width;
	int h = a->height;
	float *data = a->data;
	for (j=1; j<h-1; j++) {
		for (i=1; i<w-1; i++) {
			sum += data[IX(i,j,w)];
		}
	}
	return sum;
}

float stripe_array_sum_inner(stripe_array_t *a)
{
	int i;
	float sum = 0.0;
	if (a == NULL)
		return sum;
	for (i=0; i < a->len; i++) {
		sum += stripe_sum_inner(a->stripes[i]);
	}
	return sum;
}
void stripe_set_bounds(stripe_t *stripe)
{
	int i, j;
	int size;
	if (stripe == NULL)
		return;
	stripe_t s = *stripe;
	int w = s.width;
	int h = s.height;
	for (i=1; i < w-1; i++) {
		s.data[IX(i,0,w)] = s.data[IX(i,1,w)];
	}
	for (j=1; j < h-1; j++) {
		s.data[IX(0,j,w)] = s.data[IX(1,j,w)];
		s.data[IX(w-1,j,w)] = s.data[IX(w-2,j,w)];
	}
	for (i=1; i < w-1; i++) {
		s.data[IX(i,h-1,w)] = s.data[IX(i,h-2,w)];
	}
	// corners
	s.data[IX(0,0,w)]   = s.data[IX(1,1,w)];
	s.data[IX(w-1,0,w)] = s.data[IX(w-2,1,w)];
	s.data[IX(0,h-1,w)] = s.data[IX(1,h-2,w)];
	s.data[IX(w-1,h-1,w)] = s.data[IX(w-2,h-2,w)];
}

void dump_stripe(stripe_t *stripe)
{
	int x, y;
	if (stripe == NULL)
		return;
	stripe_t s = *stripe;
	for(y = 0; y < s.height; y++){
		for(x = 0; x < s.width; x++) {
			int index = y * s.width + x;
			printf("%#6.2f ", s.data[index]);
		}
		printf("\n");
	}
}

float stripe_diff(stripe_t *s1, stripe_t *s2)
{
	float max = 0.0;
	float min = 0.0;
	float sum = 0.0;
	float avg = 0.0;
	float span = 0.0;
	int i;
	int size;
	if (s1 == NULL || s2 == NULL || s1->data == NULL || s2 ->data == NULL)
		return UINT_MAX;
	if (s1->width != s2->width || s1->height != s2->height)
		return UINT_MAX;
	size = s1->height * s1->width;
	stripe_t l1 = *s1;
	stripe_t l2 = *s2;
	for (i=0; i < size; i++) {
		sum += abs(l1.data[i] - l2.data[i]);
		if (l1.data[i] > max)
			max = l1.data[i];
		if (l2.data[i] > max)
			max = l2.data[i];
		if (l1.data[i] < min)
			min = l1.data[i];
		if (l2.data[i] < min)
			min = l2.data[i];
	}
	span = abs(max - min);
	avg = sum / size;
	return avg / span;
}

/* exchange bounds of s1 and s2
 * it copies the last inner line of s1 to top padding of s2
 * and copies the first inner line of s2 to bottom padding of s1 */
void stripe_xchg_bounds(stripe_t *s1, stripe_t *s2)
{
	int i, w;
	int sl1, dl1, sl2, dl2;
	if (s1 == NULL || s2 == NULL || s1->data == NULL || s2 ->data == NULL)
		return ;
	if (s1->width != s2->width)
		return;
	if (s1->width < 2 || s2->width < 2)
		return;
	stripe_t top = *s1;
	stripe_t bot = *s2;
	w = top.width;
	float tmp;
	for (i = 0; i < w; i++) {
		tmp = bot.data[IX(i,0,w)];
		bot.data[IX(i,0,w)]            = top.data[IX(i,top.height-1,w)];
		top.data[IX(i,top.height-1,w)] = tmp;
	}
}

void foreach_stripe_0(stripe_array_t *s, void(*func)(stripe_t *))
{
	//printf("foreach_stripe_0\n");
	int i;
	if (s == NULL)
		return;
	for (i=0; i<s->len; i++) {
		//printf("stripe %d\n", i);
		func(s->stripes[i]);
	}
}

void foreach_stripe_1(stripe_array_t *s, void(*func)(stripe_t *, float), float f)
{
	//printf("foreach_stripe_1\n");
	int i;
	if (s == NULL)
		return;
	for (i=0; i<s->len; i++)
		func(s->stripes[i], f);
}
