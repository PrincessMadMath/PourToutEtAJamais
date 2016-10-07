/*
 * fluid.c
 *
 *  Created on: 2011-10-05
 *      Author: francis
 */

#include <stdlib.h>
#include <stdio.h>
#include "color.h"
#include "solver.h"

#define IX(i,j) ((i)+(N+2)*(j))

/* global variables */

static int N;
static float dt, diff, visc;
static float force, source;
static int dvel;

static float * u, * v, * u_prev, * v_prev;
static float * dens, * dens_prev;

static void free_data ( void )
{
	if ( u ) free ( u );
	if ( v ) free ( v );
	if ( u_prev ) free ( u_prev );
	if ( v_prev ) free ( v_prev );
	if ( dens ) free ( dens );
	if ( dens_prev ) free ( dens_prev );
}

static void clear_data ( void )
{
	int i, size=(N+2)*(N+2);

	for ( i=0 ; i<size ; i++ ) {
		u[i] = v[i] = u_prev[i] = v_prev[i] = dens[i] = dens_prev[i] = 0.0f;
	}
}

static int allocate_data ( void )
{
	int size = (N+2)*(N+2);

	u			= (float *) malloc ( size*sizeof(float) );
	v			= (float *) malloc ( size*sizeof(float) );
	u_prev		= (float *) malloc ( size*sizeof(float) );
	v_prev		= (float *) malloc ( size*sizeof(float) );
	dens		= (float *) malloc ( size*sizeof(float) );
	dens_prev	= (float *) malloc ( size*sizeof(float) );

	if ( !u || !v || !u_prev || !v_prev || !dens || !dens_prev ) {
		fprintf ( stderr, "cannot allocate data\n" );
		return ( 0 );
	}

	return ( 1 );
}

void dens_image(struct rgb *image, float *dens)
{
	int i, j;
	for(i = 0; i < N+2; i++) {
		for (j = 0; j < N+2; j++) {
			int index = IX(i,j);
			struct rgb *pix = &image[index];
			pix->r = pix->g = pix->b = (unsigned char) dens[index] * 255 / source;
		}
	}
}

void set(void)
{
	int i, j, k;
	int m = (N+2)/2 - 50;
	int w = 5;
	for (k = 0; k < 3; k++) {
		for (i = m - w; i < m + w; i++) {
			for (j = 200; j < 210; j++) {
				dens_prev[IX(j,i)] = source * 10;
			}
		}
		m += 50;
	}
	for (i = m; i < m + 100; i++) {
		for (j = 5; j < 15; j++) {
			u_prev[IX(j,i)] = 1;
		}
	}
}

int main(int argc, char **argv)
{
	N = 512;
	dt = 0.1f;
	diff = 0.0f;
	visc = 0.0f;
	force = 5.0f;
	source = 100.0f;

	int k,i,j;
	int size = (N+2)*(N+2);
	struct rgb *image = malloc(sizeof(struct rgb) * size);
	char path[1024];
	allocate_data();
	clear_data();
	for (k = 0; k < 20; k++) {
		set();
		vel_step ( N, u, v, u_prev, v_prev, visc, dt );
		dens_step ( N, dens, dens_prev, u, v, diff, dt );
		dens_image(image, dens);
		sprintf(path, "dens-%03d.ppm", k);
		save_image(path, image, N+2, N+2);
	}

	free_data();
	return 0;
}
