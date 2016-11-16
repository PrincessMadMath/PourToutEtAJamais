/*
 * mpi5.c
 *
 *  Created on: 2011-11-08
 *      Author: francis
 */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>
#include "memory.h"

double f(double a)
{
	return 4.0 / (1.0 + a * a);
}

int main(int argc, char **argv)
{
	int numprocs, rank, i;
	int precision = 0;
	double h, sum, x, mypi, pi;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (rank == 0) {
		precision = 10;
		if (argc > 1) {
			precision = atoi(argv[1]);
		}
	}
	/* broadcast precision to all process */
	MPI_Bcast(&precision, 1, MPI_INTEGER, 0, MPI_COMM_WORLD);

	/* calculate the interval size */
	sum = 0.0;
	h = 1.0 / (double) precision;
	for (i = rank + 1; i <= precision; i += numprocs) {
		x = h * ((double) i - 0.5);
		sum += f(x);
	}
	mypi = h * sum;

	/* Reduction of partial computations */
	MPI_Reduce(&mypi, &pi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

	/* verify result */
	if (rank == 0) {
		printf("pi=%.16f error=%.16f\n", pi, fabs(pi - M_PI));
	}
	MPI_Finalize();
	return 0;
}
