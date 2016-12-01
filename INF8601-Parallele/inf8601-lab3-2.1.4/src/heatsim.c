/*
 * heatsim.c
 *
 *  Created on: 2011-11-17
 *      Author: francis
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>
#include <limits.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>

#include "config.h"
#include "part.h"
#include "grid.h"
#include "cart.h"
#include "image.h"
#include "heat.h"
#include "memory.h"
#include "util.h"

#define PROGNAME "heatsim"
#define DEFAULT_OUTPUT_PPM "heatsim.png"
#define DEFAULT_DIMX 1
#define DEFAULT_DIMY 1
#define DEFAULT_ITER 100
#define MAX_TEMP 1000.0
#define DIM_2D 2

typedef struct ctx {
	cart2d_t *cart;
	grid_t *global_grid;
	grid_t *curr_grid;
	grid_t *next_grid;
	grid_t *heat_grid;
	int numprocs;
	int rank;
	MPI_Comm comm2d;
	FILE *log;
	int verbose;
	int dims[DIM_2D];
	int isperiodic[DIM_2D];
	int coords[DIM_2D];
	int reorder;
	int north_peer;
	int south_peer;
	int east_peer;
	int west_peer;
	MPI_Datatype vector;
} ctx_t;

typedef struct command_opts {
	int dimx;
	int dimy;
	int iter;
	char *input;
	char *output;
	int verbose;
} opts_t;

static opts_t *global_opts = NULL;

__attribute__((noreturn))
static void usage(void) {
	fprintf(stderr, PROGNAME " " VERSION " " PACKAGE_NAME "\n");
	fprintf(stderr, "Usage: " PROGNAME " [OPTIONS] [COMMAND]\n");
	fprintf(stderr, "\nOptions:\n");
	fprintf(stderr, "  --help	this help\n");
	fprintf(stderr, "  --iter	number of iterations to perform\n");
	fprintf(stderr, "  --dimx	2d decomposition in x dimension\n");
	fprintf(stderr, "  --dimy	2d decomposition in y dimension\n");
	fprintf(stderr, "  --input  png input file\n");
	fprintf(stderr, "  --output ppm output file\n");
	fprintf(stderr, "\n");
	exit(EXIT_FAILURE);
}

static void dump_opts(struct command_opts *opts) {
	printf("%10s %s\n", "option", "value");
	printf("%10s %d\n", "dimx", opts->dimx);
	printf("%10s %d\n", "dimy", opts->dimy);
	printf("%10s %d\n", "iter", opts->iter);
	printf("%10s %s\n", "input", opts->input);
	printf("%10s %s\n", "output", opts->output);
	printf("%10s %d\n", "verbose", opts->verbose);
}

void default_int_value(int *val, int def) {
	if (*val == 0)
		*val = def;
}

static int parse_opts(int argc, char **argv, struct command_opts *opts) {
	int idx;
	int opt;
	int ret = 0;

	struct option options[] = { { "help", 0, 0, 'h' },
			{ "iter", 1, 0, 'r' }, { "dimx", 1, 0, 'x' }, { "dimy",
					1, 0, 'y' }, { "input", 1, 0, 'i' }, {
					"output", 1, 0, 'o' }, { "verbose", 0,
					0, 'v' }, { 0, 0, 0, 0 } };

	memset(opts, 0, sizeof(struct command_opts));

	while ((opt = getopt_long(argc, argv, "hvx:y:l:", options, &idx)) != -1) {
		switch (opt) {
		case 'r':
			opts->iter = atoi(optarg);
			break;
		case 'y':
			opts->dimy = atoi(optarg);
			break;
		case 'x':
			opts->dimx = atoi(optarg);
			break;
		case 'i':
			if (asprintf(&opts->input, "%s", optarg) < 0)
				goto err;
			break;
		case 'o':
			if (asprintf(&opts->output, "%s", optarg) < 0)
				goto err;
			break;
		case 'h':
			usage();
			break;
		case 'v':
			opts->verbose = 1;
			break;
		default:
			printf("unknown option %c\n", opt);
			ret = -1;
			break;
		}
	}

	/* default values*/
	default_int_value(&opts->iter, DEFAULT_ITER);
	default_int_value(&opts->dimx, DEFAULT_DIMX);
	default_int_value(&opts->dimy, DEFAULT_DIMY);
	if (opts->output == NULL)
		if (asprintf(&opts->output, "%s", DEFAULT_OUTPUT_PPM) < 0)
			goto err;
	if (opts->input == NULL) {
		fprintf(stderr, "missing input file");
		goto err;
	}

	if (opts->dimx == 0 || opts->dimy == 0) {
		fprintf(stderr,
				"argument error: dimx and dimy must be greater than 0\n");
		ret = -1;
	}

	if (opts->verbose)
		dump_opts(opts);
	global_opts = opts;
	return ret;
	err:
	FREE(opts->input);
	FREE(opts->output);
	return -1;
}

FILE *open_logfile(int rank) {
	char str[255];
	sprintf(str, "out-%d", rank);
	FILE *f = fopen(str, "w+");
	return f;
}

ctx_t *make_ctx() {
	ctx_t *ctx = (ctx_t *) calloc(1, sizeof(ctx_t));
	return ctx;
}

void free_ctx(ctx_t *ctx) {
	if (ctx == NULL)
		return;
	free_grid(ctx->global_grid);
	free_grid(ctx->curr_grid);
	free_grid(ctx->next_grid);
	free_grid(ctx->heat_grid);
	free_cart2d(ctx->cart);
	if (ctx->log != NULL) {
		fflush(ctx->log);
		fclose(ctx->log);
	}
	FREE(ctx);
}

int init_ctx(ctx_t *ctx, opts_t *opts) {

	//printf("Starting init_ctx\n");

	MPI_Comm_size(MPI_COMM_WORLD, &ctx->numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &ctx->rank);

	if (opts->dimx * opts->dimy != ctx->numprocs) {
		fprintf(stderr,
				"2D decomposition blocks must equal number of process\n");
		goto err;
	}
	ctx->log = open_logfile(ctx->rank);
	ctx->verbose = opts->verbose;
	ctx->dims[0] = opts->dimx;
	ctx->dims[1] = opts->dimy;
	ctx->isperiodic[0] = 1;
	ctx->isperiodic[1] = 1;
	ctx->reorder = 0;
	grid_t *new_grid = NULL;
	size_t nbProcess = ctx->numprocs;
	
	// Create 2D Map
	MPI_Cart_create(MPI_COMM_WORLD, DIM_2D, ctx->dims, ctx->isperiodic, ctx->reorder, &ctx->comm2d);
	MPI_Cart_shift(ctx->comm2d, 1, 1, &ctx->north_peer, &ctx->south_peer);
	MPI_Cart_shift(ctx->comm2d, 0, 1, &ctx->west_peer, &ctx->east_peer);
	MPI_Cart_coords(ctx->comm2d, ctx->rank, DIM_2D, ctx->coords);

	int messages_count_type = 4;
	int tag_wight_message = 0;
	int tag_heigth_message = 1;
	int tag_padding_message = 2;
	int tag_data_message = 3;

	// In main process
	if(ctx->rank == 0)
	{
	  /* load input image */
	  image_t *image = load_png(opts->input);
	  if (image == NULL)
	    goto err;
	  
	  /* select the red channel as the heat source */
	  ctx->global_grid = grid_from_image(image, CHAN_RED);
	  
	  /* grid is normalized to one, multiply by MAX_TEMP */
	  grid_multiply(ctx->global_grid, MAX_TEMP);
	  
	  /* 2D decomposition */
	  ctx->cart = make_cart2d(ctx->global_grid->width, ctx->global_grid->height, opts->dimx, opts->dimy);
	  cart2d_grid_split(ctx->cart, ctx->global_grid);
	  
	  if(nbProcess > 1)
	  {
		// Number of messages to send
		int messages_count = messages_count_type*(nbProcess-1);
		
		MPI_Request *req = calloc(messages_count, sizeof(MPI_Request));
		MPI_Status *status = calloc(messages_count, sizeof(MPI_Status));
		
	    int process_index;
	    for(process_index = 1; process_index < nbProcess; ++process_index)
	    {
	    	// Get coords
			int processCoordinates[DIM_2D];
			MPI_Cart_coords(ctx->comm2d, process_index, DIM_2D, processCoordinates);	

			// Get grid
			grid_t *grid = cart2d_get_grid(ctx->cart, processCoordinates[0], processCoordinates[1]);	

			//Calculate shift for request
			int process_shift = messages_count_type*(process_index-1);

			/*
			printf("Sending to %i width: %i\n", process_index, grid->width);
			printf("Sending to %i height: %i\n", process_index, grid->height);
			printf("Sending to %i padding: %i\n", process_index, grid->padding);
			*/

			// Sending grid meta-data
			MPI_Isend(&grid->width, 1, MPI_INTEGER, process_index, tag_wight_message, ctx->comm2d, req + process_shift + tag_wight_message);
			MPI_Isend(&grid->height, 1, MPI_INTEGER, process_index, tag_heigth_message, ctx->comm2d, req + process_shift + tag_heigth_message);
			MPI_Isend(&grid->padding, 1 , MPI_INTEGER, process_index, tag_padding_message, ctx->comm2d, req + process_shift + tag_padding_message);

			// Sending grid data
	      	MPI_Isend(grid->dbl, grid->pw*grid->ph, MPI_DOUBLE, process_index, tag_data_message, ctx->comm2d, req + process_shift + tag_data_message);
	    }

	    // Wait for all messages to be sent
	    MPI_Waitall(messages_count, req, status);
	   	free(req);
	    free(status);
	  }
	  // Calculate grid on main computer
 	  int mainProcess_coords[DIM_2D];
	  MPI_Cart_coords(ctx->comm2d, ctx->rank, DIM_2D, mainProcess_coords);
	  new_grid = cart2d_get_grid(ctx->cart, mainProcess_coords[0], mainProcess_coords[1]);

	}
	else
	{	  
	    MPI_Request request[messages_count_type];
        MPI_Status status[messages_count_type];
        
        // Receiving grid meta-data
		int width, height, padding;

		MPI_Irecv(&width, 1, MPI_INTEGER, 0, tag_wight_message, ctx->comm2d, &request[0]);
		MPI_Irecv(&height, 1, MPI_INTEGER, 0, tag_heigth_message, ctx->comm2d, &request[1]);
		MPI_Irecv(&padding, 1, MPI_INTEGER, 0, tag_padding_message, ctx->comm2d, &request[2]);
		MPI_Waitall(3, request, status);

		/*
		printf("Received width: %i\n", width);
		printf("Received height: %i\n", height);
		printf("Received padding: %i\n", padding);
		*/
		
		// Initializing grid with received meta-data
		new_grid = make_grid(width, height, padding);

		// Receiving grid data
		MPI_Irecv(new_grid->dbl, new_grid->pw*new_grid->ph, MPI_DOUBLE, 0, tag_data_message, ctx->comm2d, &request[3]);
		MPI_Waitall(1, &request[3], &status[3]);
    }

	if (new_grid == NULL)
		goto err;
		
	/* set padding required for Runge-Kutta */
	ctx->curr_grid = grid_padding(new_grid, 1);
	ctx->next_grid = grid_padding(new_grid, 1);
	ctx->heat_grid = grid_padding(new_grid, 1);

	MPI_Type_vector(ctx->curr_grid->height, 1, ctx->curr_grid->pw, MPI_DOUBLE, &ctx->vector);
	MPI_Type_commit(&ctx->vector);

	//printf("Ending init_ctx\n");

	return 0;
	err: return -1;
}

void dump_ctx(ctx_t *ctx) {
	fprintf(ctx->log, "*** CONTEXT ***\n");
	fprintf(ctx->log, "rank=%d\n", ctx->rank);
	fprintf(ctx->log, "north=%d south=%d west=%d east=%d \n",
			ctx->north_peer, ctx->south_peer,
			ctx->east_peer, ctx->west_peer);
	fprintf(ctx->log, "***************\n");
}

void exchng2d(ctx_t *ctx){

	// Get current grid meta-data and data
	grid_t *grid = ctx->curr_grid;
	int width = grid->width;
	int process_width = grid->pw;
	int process_height = grid->ph;
	int padding = grid->padding;

	MPI_Request req[8];
	MPI_Status status[8];

	// Calculate shifting for neighbors
	double *north_send_buffer = grid->dbl + process_width * padding + padding;
	double *south_send_buffer = grid->dbl + (process_height - padding - 1)*process_width + padding;
	double *east_send_buffer = north_send_buffer + width - 1;
	double *west_send_buffer = north_send_buffer;

	double *north_recv_buffer = north_send_buffer - process_width;
	double *south_recv_buffer = south_send_buffer + process_width;
	double *east_recv_buffer = east_send_buffer + 1;
	double *west_recv_buffer = west_send_buffer - 1;

	/*
	printf("****** I am %i *********\n", ctx->rank);
	printf("North peer is %i\n", ctx->north_peer);
	printf("South peer is %i\n", ctx->south_peer);
	printf("East peer is %i\n", ctx->east_peer);
	printf("West peer is: to %i\n", ctx->west_peer);
	*/	

	// Receiving from neighbors
	MPI_Irecv(north_recv_buffer, width, MPI_DOUBLE, ctx->north_peer,11, ctx->comm2d, &req[0]);
	MPI_Irecv(south_recv_buffer, width, MPI_DOUBLE, ctx->south_peer,10, ctx->comm2d, &req[1]);
	MPI_Irecv(east_recv_buffer, 1, ctx->vector,	ctx->east_peer,	13, ctx->comm2d, &req[2]);
	MPI_Irecv(west_recv_buffer, 1, ctx->vector, ctx->west_peer,	12, ctx->comm2d, &req[3]);

	// Sending from neighbors
  	MPI_Isend(north_send_buffer, width, MPI_DOUBLE, ctx->north_peer,10, ctx->comm2d, &req[4]);
  	MPI_Isend(south_send_buffer, width, MPI_DOUBLE, ctx->south_peer,11, ctx->comm2d, &req[5]);
	MPI_Isend(east_send_buffer, 1, ctx->vector, ctx->east_peer, 12, ctx->comm2d, &req[6]);
	MPI_Isend(west_send_buffer, 1, ctx->vector, ctx->west_peer,	13, ctx->comm2d, &req[7]);

	// Wait for all sends and received to be complete
	MPI_Waitall(8, req, status);
	
}

int gather_result(ctx_t *ctx, opts_t *opts) {

	int ret = 0;

	grid_t *local_grid = grid_padding(ctx->next_grid, 0);
	if (local_grid == NULL)
		goto err;

	int tag_gather = 0;

	// Master receive result of slave
	if(ctx->rank == 0)
	{
		grid_t* grid;

		int other_process_count = ctx->numprocs-1;
		
		// Receiving others process grids
		if(other_process_count > 0)
		{	
			MPI_Request *req = (MPI_Request *) calloc(other_process_count, sizeof(MPI_Request));
			MPI_Status *status = (MPI_Status *) calloc(other_process_count, sizeof(MPI_Status));


		    int process_index;
		    for (process_index = 1; process_index <= other_process_count; ++process_index)
		    {
		    	// Get coords
		    	int slave_coords[DIM_2D];
		        MPI_Cart_coords(ctx->comm2d, process_index, DIM_2D, slave_coords);

		        // Get grid
		        grid = cart2d_get_grid(ctx->cart, slave_coords[0], slave_coords[1]);

		        // Get grid for this process
		        int grid_size = grid->height*grid->width;
		        MPI_Irecv(grid->dbl, grid_size, MPI_DOUBLE, process_index, tag_gather, ctx->comm2d, req + (process_index - 1));
		    }

			MPI_Waitall(other_process_count, req, status);

			free(req);
			free(status);

		}

		grid_t* master_grid;

		// Get coord
		int master_coords[DIM_2D];
		MPI_Cart_coords(ctx->comm2d, 0, DIM_2D, master_coords);

		// Get master grid
		master_grid = cart2d_get_grid(ctx->cart, master_coords[0], master_coords[1]);
		grid_copy(ctx->next_grid, master_grid);

		// Merge all data in global grid
		cart2d_grid_merge(ctx->cart, ctx->global_grid);

		free(master_grid);

	}
	// Slave send data to master (0)
    else
    {
        MPI_Request req;
        MPI_Status status;

        // Send grid to master
        int grid_size = local_grid->height*local_grid->width;
        MPI_Isend(local_grid->dbl, grid_size, MPI_DOUBLE, 0, tag_gather, ctx->comm2d, &req);
        MPI_Waitall(1, &req, &status);
    }


	done: free_grid(local_grid);
		return ret;
	err: ret = -1;
	goto done;
}




int main(int argc, char **argv) {
	ctx_t *ctx = NULL;
	int rep, ret;
	opts_t opts;

	if (parse_opts(argc, argv, &opts) < 0) {
		printf("Error while parsing arguments\n");
		usage();
	}
	if (opts.verbose)
		dump_opts(&opts);

	MPI_Init(&argc, &argv);

	ctx = make_ctx();
	if (init_ctx(ctx, &opts) < 0)
		goto err;
	if (opts.verbose)
		dump_ctx(ctx);

	if (ctx->verbose) {
		fprintf(ctx->log, "heat grid\n");
		fdump_grid(ctx->heat_grid, ctx->log);
	}

	for (rep = 0; rep < opts.iter; rep++) {
		if (ctx->verbose) {
			fprintf(ctx->log, "iter %d\n", rep);
			fprintf(ctx->log, "start\n");
			fdump_grid(ctx->curr_grid, ctx->log);
		}

		grid_set_min(ctx->heat_grid, ctx->curr_grid);
		if (ctx->verbose) {
			fprintf(ctx->log, "grid_set_min\n");
			fdump_grid(ctx->curr_grid, ctx->log);
		}

		exchng2d(ctx);
		if (ctx->verbose) {
			fprintf(ctx->log, "exchng2d\n");
			fdump_grid(ctx->curr_grid, ctx->log);
		}

		heat_diffuse(ctx->curr_grid, ctx->next_grid);
		if (ctx->verbose) {
			fprintf(ctx->log, "heat_diffuse\n");
			fdump_grid(ctx->next_grid, ctx->log);
		}
		SWAP(ctx->curr_grid, ctx->next_grid);
	}

	MPI_Barrier(MPI_COMM_WORLD);
	if (gather_result(ctx, &opts) < 0)
		goto err;

	if (ctx->rank == 0) {
		printf("saving...\n");
		if (save_grid_png(ctx->global_grid, opts.output) < 0) {
			printf("saving failed\n");
			goto err;
		}
	}

	MPI_Barrier(MPI_COMM_WORLD);
	ret = EXIT_SUCCESS;
done:
	free_ctx(ctx);
	MPI_Finalize();
	FREE(opts.input);
	FREE(opts.output);
	return ret;
err:
	MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
	ret = EXIT_FAILURE;
	goto done;
}

