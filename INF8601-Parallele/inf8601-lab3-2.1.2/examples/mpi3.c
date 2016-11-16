/*
 * mpi3.c
 *
 *  Created on: 2011-11-08
 *      Author: francis
 */

#include <stdio.h>
#include <mpi.h>

#define MSG_SIZE 128

int main(int argc, char **argv)
{
	int numprocs, rank, namelen;
	MPI_Status status;
	MPI_Request request;
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	char msg0[MSG_SIZE] = "foo";
	char msg1[MSG_SIZE] = "bar";
	char msg_recv[MSG_SIZE];

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (numprocs != 2) {
		printf("Error: number of processors must be equal to 2\n");
		MPI_Finalize();
		return -1;
	}

	switch (rank) {
	case 0:
		MPI_Send(msg0, MSG_SIZE, MPI_CHARACTER, 1, 1, MPI_COMM_WORLD);
		MPI_Irecv(msg_recv, MSG_SIZE, MPI_CHARACTER, 1, 1, MPI_COMM_WORLD,
				&request);
		break;
	case 1:
		MPI_Irecv(msg_recv, MSG_SIZE, MPI_CHARACTER, 0, 1, MPI_COMM_WORLD,
				&request);
		MPI_Send(msg1, MSG_SIZE, MPI_CHARACTER, 0, 1, MPI_COMM_WORLD);
		break;
	default:
		break;
	}
	MPI_Get_processor_name(processor_name, &namelen);
	MPI_Wait(&request, &status);
	printf("Process %d on %s out of %d recv=%s\n", rank, processor_name,
			numprocs, msg_recv);
	MPI_Finalize();
	return 0;
}
