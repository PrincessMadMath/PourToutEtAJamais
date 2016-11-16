/*
 * mpi7.c
 *
 *  Created on: 2011-11-10
 *      Author: francis
 */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>
#include <limits.h>

#define CHUNKSIZE 1000
#define REQUEST 1
#define REPLY 2

int main(int argc, char **argv)
{
	MPI_Comm world, workers;
	MPI_Group world_group, worker_group;
	MPI_Status status;
	int ranks[1];
	int rands[CHUNKSIZE], request;
	int numprocs, rank, server;
	int iter, in, out, i, max, done, workerid;
	double x, y, Pi, error, epsilon, totalin, totalout;
	Pi = 0.0;

	MPI_Init(&argc, &argv);
	world = MPI_COMM_WORLD;
	MPI_Comm_size(world, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	/* use the last process as the server */
	server = numprocs - 1;

		/* create world_group with all process from world communicator */
	MPI_Comm_group(world, &world_group);

	/* worker_group is all process from world_group, server excluded */
	ranks[0] = server;
	MPI_Group_excl(world_group, 1, ranks, &worker_group);

	/* create workers communicator with the worker_group members */
	MPI_Comm_create(world, worker_group, &workers);

	/* free groups, they are not needed anymore
	 * only communicators are used from now on */
	MPI_Group_free(&worker_group);
	MPI_Group_free(&world_group);

	/* broadcast epsilon */
	if (rank == 0) {
		epsilon = 0.0001;
	}
	MPI_Bcast(&epsilon, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	if (rank == server) {
		do {
			MPI_Recv(&request, 1, MPI_INTEGER, MPI_ANY_SOURCE, REQUEST, world, &status);
			if (request) {
				for (i=0; i<CHUNKSIZE;) {
					rands[i] = random();
					if (rands[i] <= INT_MAX) i++;
				}
				MPI_Send(rands, CHUNKSIZE, MPI_INTEGER, status.MPI_SOURCE, REPLY, world);
			}
		} while(request > 0);
	} else {
		request = 1;
		done = in = out = 0;
		max = INT_MAX;
		MPI_Send(&request, 1, MPI_INTEGER, server, REQUEST, world);
		MPI_Comm_rank(workers, &workerid);
		iter = 0;
		while(!done) {
			iter++;
			request = 1;
			MPI_Recv(rands, CHUNKSIZE, MPI_INTEGER, server, REPLY, world, &status);
			for (i=0; i<CHUNKSIZE;) {
				x = (((double) rands[i++])/max) * 2 - 1;
				y = (((double) rands[i++])/max) * 2 - 1;
				if ((x*x + y*y < 1.0)) {
					in++;
				} else {
					out++;
				}
			}
			MPI_Allreduce(&in, &totalin, 1, MPI_INTEGER, MPI_SUM, workers);
			MPI_Allreduce(&out, &totalout, 1, MPI_INTEGER, MPI_SUM, workers);
			Pi = (4.0*totalin) / (totalin + totalout);
			error = fabs(Pi - M_PI);
			done = (error < epsilon || (totalin + totalout) > 1000000);
			request = (done) ? 0 : 1;
			if (rank == 0) {
				MPI_Send(&request, 1, MPI_INTEGER, server, REQUEST, world);
			} else {
				if (request)
					MPI_Send(&request, 1, MPI_INTEGER, server, REQUEST, world);
			}
		}
		MPI_Comm_free(&workers);
	}
	if (rank == 0)
	    printf("pi=%23.20f error=%23.20f\n", Pi, fabs(Pi - M_PI));
	MPI_Finalize();
	return 0;
}
