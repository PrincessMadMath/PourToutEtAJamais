/*
 * mpi6.c
 *
 *  Created on: 2011-11-08
 *      Author: francis
 */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>
#include "part.h"
#include "memory.h"

#define MAX_ROWS 1000
#define MAX_COLS 1000
#define END_TAG 9999

void init(int *a, int *b, int rows, int cols)
{
	int i, j;
	for (j = 0; j < rows; j++) {
		b[j] = 1;
		for (i = 0; i < cols; i++) {
			int index = j * cols + i;
			a[index] = index;
		}
	}
}

int main(int argc, char **argv)
{
	int numprocs, rank, i;
	int cols, rows, anstype, row, numsent, sender;
	MPI_Status status;
	int a[MAX_ROWS * MAX_COLS];
	int b[MAX_COLS];
	int c[MAX_ROWS];
	int buffer[MAX_COLS];
	int ans;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	cols = 10;
	rows = 10;

	if (rank == 0) {
		numsent = 0;
		init(a, b, rows, cols);
		/* send b to all process */
		MPI_Bcast(b, cols, MPI_INTEGER, 0, MPI_COMM_WORLD);

		/* send a row to each slave process, tag with row number */
		int tosend = numprocs < rows ? numprocs : rows;
		int offset;
		for (i = 1; i < tosend; i++) {
			offset = numsent * cols;
			MPI_Send(a + offset, cols, MPI_INTEGER, i, i - 1, MPI_COMM_WORLD);
			numsent++;
		}

		/* process remaining rows */
		for (i = 0; i < rows; i++) {
			MPI_Recv(&ans, 1, MPI_INTEGER, MPI_ANY_SOURCE, MPI_ANY_TAG,
					MPI_COMM_WORLD, &status);
			sender = status.MPI_SOURCE;
			anstype = status.MPI_TAG;
			c[anstype] = ans;
			if (numsent < rows) {
				offset = numsent * cols;
				MPI_Send(a + offset, cols, MPI_INTEGER, sender, numsent,
						MPI_COMM_WORLD);
				numsent++;
			} else {
				MPI_Send(MPI_BOTTOM, 0, MPI_INTEGER, sender, END_TAG,
						MPI_COMM_WORLD);
			}
		}

	} else {
		/* receive b and compute the dot product */
		MPI_Bcast(b, cols, MPI_INTEGER, 0, MPI_COMM_WORLD);

		/* perform work only if there is more rows than our rank */
		if (rank >= rows) {
			goto end;
		}
		while (1) {
			MPI_Recv(buffer, cols, MPI_INTEGER, 0, MPI_ANY_TAG, MPI_COMM_WORLD,
					&status);
			if (status.MPI_TAG == END_TAG) {
				goto end;
			}
			row = status.MPI_TAG;
			ans = 0;
			for (i = 0; i < cols; i++) {
				ans += buffer[i] * b[i];
			}
			MPI_Send(&ans, 1, MPI_INTEGER, 0, row, MPI_COMM_WORLD);
		}
	}

	end: if (rank == 0)
		print_array(c, rows);
	MPI_Finalize();
	return 0;
}
