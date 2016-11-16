/*
 * mpi4.c
 *
 *  Created on: 2011-11-08
 *      Author: francis
 */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "memory.h"
#include "part.h"

#define DIM_GLOBAL 100
#define DIM_LOCAL 10

int main(int argc, char **argv)
{
	int numprocs, rank, namelen, i;
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	int *vx = NULL, *vy = NULL, *vz = NULL, *vxpart = NULL, *vypart = NULL,
			*vzpart = NULL, coeff[2];
	int exp = 0, act = 0;
	int *count = NULL;
	int *disp = NULL;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	/* let the main process initialize the data */
	if (rank == 0) {
		vx = (int *) malloc(sizeof(int) * DIM_GLOBAL);
		vy = (int *) malloc(sizeof(int) * DIM_GLOBAL);
		vz = (int *) malloc(sizeof(int) * DIM_GLOBAL);

		for (i = 0; i < DIM_GLOBAL; i++) {
			vx[i] = i;
			vy[i] = i;
			vz[i] = 0;
			exp += 2 * i + 3 * i;
		}
		coeff[0] = 2;
		coeff[1] = 3;
	}

	/* compute size of chunks */
	sendcounts_array(&count, numprocs, DIM_GLOBAL);
	displs_array(&disp, count, numprocs);

	/* allocate work buffer to all, including the master process */
	vxpart = (int *) malloc(sizeof(int) * count[rank]);
	vypart = (int *) malloc(sizeof(int) * count[rank]);
	vzpart = (int *) malloc(sizeof(int) * count[rank]);

	/* Scatter the data to peers */
	MPI_Scatterv(vx, count, disp, MPI_INTEGER, vxpart, count[rank],
			MPI_INTEGER, 0, MPI_COMM_WORLD);
	MPI_Scatterv(vy, count, disp, MPI_INTEGER, vypart, count[rank],
			MPI_INTEGER, 0, MPI_COMM_WORLD);

	/* Broadcast is done here because coeff is the same for all computations */
	MPI_Bcast(coeff, 2, MPI_INTEGER, 0, MPI_COMM_WORLD);

	/* perform the actual computation */
	for (i = 0; i < count[rank]; i++) {
		vzpart[i] = coeff[0] * vxpart[i] + coeff[1] * vypart[i];
	}

	/* Gather the results */
	MPI_Gatherv(vzpart, count[rank], MPI_INTEGER, vz, count, disp, MPI_INTEGER,
			0, MPI_COMM_WORLD);

	/* verify result */

	if (rank == 0) {
		for (i = 0; i < DIM_GLOBAL; i++) {
			act += vz[i];
		}
		printf("exp=%d act=%d\n", exp, act);
	}
	if (rank == 0) {
		FREE(vx);
		FREE(vy);
		FREE(vz);
	}
	FREE(vxpart);
	FREE(vypart);
	FREE(vzpart);
	FREE(disp);
	FREE(count);
	MPI_Get_processor_name(processor_name, &namelen);
	MPI_Finalize();
	return 0;
}
