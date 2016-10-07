/*
 * Heat diffusion simulation with OpenMP and OpenCL
 */

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <errno.h>
#include <png.h>
#include <math.h>

#include "layer.h"
#include "color.h"
#include "util.h"
#include "heat.h"
#include "heat_cuda.h"

void diffusion_1(stripe_array_t *sa_curr, stripe_array_t *sa_next, stripe_array_t *sa_diff)
{
	int k, r, i, j;
	int width, height;
	float *curr, *next, *diff;
	if (sa_curr == NULL || sa_next == NULL || sa_diff == NULL)
		return;
	int len = sa_curr->len;
	for(r=0; r < (20 + len); r++) {
		#pragma omp parallel private(curr,next,diff,width,height,i,j,k)
		{
			#pragma omp for schedule(static,1)
			for(k=0; k < len; k++) {
				curr = sa_curr->stripes[k]->data;
				next = sa_next->stripes[k]->data;
				diff = sa_diff->stripes[k]->data;
				width = sa_curr->stripes[k]->width;
				height = sa_curr->stripes[k]->height;
				for(j=1; j < height - 1; j++) {
					for (i = 1; i < width - 1; i++) {
						int c =	IX(i, j, width);
						int n = IX(i, j-1, width);
						int s = IX(i, j+1, width);
						int w = IX(i-1, j, width);
						int e = IX(i+1, j, width);
						// epic fail
						//float sum_diff = diff[n] + diff[s] + diff[e] + diff[w];
						//next[c] = (curr[c] + diff[n] * next[n] + diff[s] * next[s] + diff[e] * next[e] + diff[w] * next[w]) / (1 + 4 * sum_diff);

						// ok, mais facteur de diffusion cst
						//next[c] = (curr[c] + diff[c]*(next[n] + next[s] + next[e] + next[w])) / (1 + 4 * diff[c]);

						// methode instable avec diffusion cst
						//next[c] = curr[c] + diff[c]*(curr[n] + curr[s] + curr[e] + curr[w] - 4*curr[c])

						// methode instable avec diffusion variable
						//float sum_diff = diff[n] + diff[s] + diff[e] + diff[w];
						//next[c] = curr[c] + sum_diff(diff[n]*curr[n] + diff[s]*curr[s] + diff[e]*curr[e] + diff[w]*curr[w] - sum_diff*curr[c])
						float bidon = (diff[n] - diff[s])*(next[n] - next[s]) + (diff[e] - diff[w])*(next[e] - next[w]);
						//bidon = 0.0;
						next[c] = (curr[c] + bidon + diff[c]*(next[n] + next[s] + next[e] + next[w])) / (1 + 4 * diff[c]);
					}
				}
				stripe_set_bounds(sa_next->stripes[k]);
			}
			#pragma omp for schedule(static, 1)
			for(k=0; k < sa_next->len-1; k++) {
				//printf("stripe_xchg_bounds\n");
				stripe_t *s1 = sa_next->stripes[k];
				stripe_t *s2 = sa_next->stripes[k+1];
				stripe_xchg_bounds(s1, s2);
			}
		}
	}
	foreach_stripe_0(sa_curr, stripe_set_bounds);
	foreach_stripe_0(sa_next, stripe_set_bounds);
}

/* assume each stripe array have the same number of stripes and they have
 * the same size */
void set_heat(stripe_array_t *curr, stripe_array_t *heat)
{
	int i, j, s, a;
	stripe_t scurr, sheat;
	if (curr == NULL || heat == NULL)
		return;
	for (s=0; s < curr->len; s++) {
		scurr = *curr->stripes[s];
		sheat = *heat->stripes[s];
		a = scurr.width * scurr.height;
		for (i=0; i < a; i++) {
			if (scurr.data[i] < sheat.data[i])
				scurr.data[i] = sheat.data[i];
		}
	}
}

void fix_heat(stripe_array_t *curr, stripe_array_t *next)
{
	float h1 = stripe_array_sum_inner(curr);
	float h2 = stripe_array_sum_inner(next);
	float fact = h1 / h2;
	//printf("h1=%f h2=%f fact=%f\n", h1, h2, fact);
	foreach_stripe_1(next, stripe_mul, fact);
}

void heat_sink(stripe_array_t *next, stripe_array_t *sink)
{
	int i, j, s, w, h;
	stripe_t snext, ssink;
	if (next == NULL || sink == NULL)
		return;
	for (s=0; s < next->len; s++) {
		snext = *next->stripes[s];
		ssink = *sink->stripes[s];
		w = snext.width;
		h = snext.height;
		for(j=1; j < h-1; j++) {
			for(i=1; i < w-1; i++) {
				int index = IX(i,j,w);
				if (snext.data[index] > 0.0f) {
					snext.data[index] -= snext.data[index] * ssink.data[index];
				}
			}
		}
	}
}

int do_simulate(struct layers *l, params_t *params)
{
	int i,j,k,t,x,y,w,h;
	params_t p;
	if (l == NULL || params == NULL)
		return -1;
	p = *params;

	stripe_array_t *heat = l->layers[LAYER_HEAT];
	stripe_array_t *diff = l->layers[LAYER_DIFF];
	stripe_array_t *sink = l->layers[LAYER_SINK];
	stripe_array_t *mat1 = l->layers[LAYER_MAT1];
	stripe_array_t *mat2 = l->layers[LAYER_MAT2];

	foreach_stripe_0(diff, dump_stripe);

	foreach_stripe_1(heat, stripe_mul, p.max_heat);
	foreach_stripe_1(diff, stripe_mul, p.max_diff);
	foreach_stripe_1(sink, stripe_mul, p.max_sink);
	foreach_stripe_1(mat1, stripe_set_all, 0.0);
	foreach_stripe_1(mat2, stripe_set_all, 0.0);
	//dump_layers(l);
	stripe_array_t *curr = mat1;
	stripe_array_t *next = mat2;

	for(t=0; t < p.iter; t++) {
		set_heat(curr, heat);
		diffusion_1(curr, next, diff);
		fix_heat(curr, next);
		//heat_sink(next, sink);
		SWAP(curr, next);
		printf("%d\n", t);
		//foreach_stripe_0(curr, dump_stripe);
	}

	return 0;
}

int save_stripe_to_image(char *path, stripe_array_t *a, float max)
{
	int i, j, k, w, h, area;
	stripe_t s;
	if (a == NULL)
		return -1;
	stripe_array_trimmed_size(a, &w, &h);
	area = w * h;
	struct rgb *image = (struct rgb *) calloc(area, sizeof(struct rgb));

	for (k=0; k < a->len; k++) {
		s = *a->stripes[k];
		for(j=1; j < s.height - 1; j++) {
			for (i=1; i < s.width-1; i++) {
				int c = IX(i,j,s.width);
				value_color(&image[c], s.data[c], max);
			}
		}
	}

	save_image("out.ppm", image, w, h);
	FREE(image);
	return 0;
}

int cmd_simulate(char *path)
{
	struct layers *l = NULL;
	if (init_layers(path, &l) < 0) {
		printf("error\n");
	}
	//dump_layers(l);
	//return 0;
	/* split layers, one for each thread */
	int num_threads = 1;
	omp_set_num_threads(num_threads);
	split_layers(l, num_threads);
	params_t p;
	p.iter = 100;
	p.max_heat = 100.0;
	p.max_diff = 10;
	p.max_sink = 0.1;
	do_simulate(l, &p);
	merge_layers(l);
	//foreach_stripe_0(l->layers[LAYER_MAT2], dump_stripe);
	save_stripe_to_image("out.ppm", l->layers[LAYER_MAT2], p.max_heat);
	free_layers(l);
	l = NULL;
	return 0;
}

int main(int argc, char **argv)
{
	//cmd_simulate(argv[1]);
	return 0;
}
