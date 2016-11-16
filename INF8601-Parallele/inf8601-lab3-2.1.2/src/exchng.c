/*
 * exchng.c
 *
 *  Created on: 2011-11-10
 *      Author: francis
 */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>
#include <limits.h>
#include <string.h>
#include <getopt.h>

#include "config.h"
#include "part.h"
#include "grid.h"
#include "memory.h"

#define PROGNAME "exchng"
#define DIM_1D 1
#define DIM_2D 2
#define DEFAULT_HEIGHT	512
#define DEFAULT_WIDTH 	512
#define DEFAULT_LIB_NAME "blocking"

typedef struct ctx {
	grid_t *grid;
	int row_start;
	int row_end;
	int numprocs;
	int rank;
	MPI_Comm comm1d;
	FILE *log;
	int verbose;
	int dims[DIM_1D];
	int isperiodic[DIM_1D];
	int coords[DIM_1D];
	int reorder;
	int north;
	int south;
	int *buffer;
	int buffer_size;
} ctx_t;

typedef void (*handler)(ctx_t *);

struct lib_def {
	const char *name;
	handler handler;
};

struct command_opts {
	const struct lib_def *lib;
	int height;
	int width;
	int verbose;
};

static struct command_opts *global_opts = NULL;

void exchng1d_blocking(ctx_t *ctx);
void exchng1d_ordered(ctx_t *ctx);
void exchng1d_combined(ctx_t *ctx);
void exchng1d_buffered(ctx_t *ctx);
void exchng1d_async(ctx_t *ctx);

static const struct lib_def libs[] = { { .name = "blocking", .handler =
		exchng1d_blocking }, { .name = "ordered", .handler =
		exchng1d_ordered }, { .name = "combined", .handler =
		exchng1d_combined }, { .name = "buffered", .handler =
		exchng1d_buffered }, { .name = "async", .handler =
		exchng1d_async }, { .name = NULL, .handler = NULL }, };

__attribute__((noreturn))
static void usage(void) {
	fprintf(stderr, PROGNAME " " VERSION " " PACKAGE_NAME "\n");
	fprintf(stderr, "Usage: " PROGNAME " [OPTIONS] [COMMAND]\n");
	fprintf(stderr, "\nOptions:\n");
	fprintf(stderr, "  --help	this help\n");
	fprintf(stderr,
			"  --lib		set the exchange type to use "
					"\n\t\t[ blocking | ordered | combined | buffered | async ]\n");
	fprintf(stderr, "  --height	set height\n");
	fprintf(stderr, "  --width	set width\n");
	fprintf(stderr, "\n");
	exit(EXIT_FAILURE);
}

static const struct lib_def *lookup_lib(const char *name) {
	int i;
	for (i = 0; libs[i].name != NULL; i++) {
		if (strcmp(libs[i].name, name) == 0)
			return &libs[i];
	}
	return NULL;
}

static void dump_opts(struct command_opts *opts) {
	printf("%10s %s\n", "option", "value");
	printf("%10s %s\n", "lib", opts->lib->name);
	printf("%10s %d\n", "width", opts->width);
	printf("%10s %d\n", "height", opts->height);
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

	struct option options[] = { { "help", 0, 0, 'h' }, { "lib", 1, 0, 'l' },
			{ "height", 1, 0, 'y' }, { "width", 1, 0, 'x' }, {
					"verbose", 0, 0, 'v' }, { 0, 0, 0, 0 } };

	memset(opts, 0, sizeof(struct command_opts));

	while ((opt = getopt_long(argc, argv, "hvx:y:l:", options, &idx)) != -1) {
		switch (opt) {
		case 'l':
			opts->lib = lookup_lib(optarg);
			if (opts->lib == NULL) {
				printf("unknown lib %s\n", optarg);
				ret = -1;
			}
			break;
		case 'y':
			opts->height = atoi(optarg);
			break;
		case 'x':
			opts->width = atoi(optarg);
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
	if (opts->lib == NULL)
		opts->lib = lookup_lib(DEFAULT_LIB_NAME);

	default_int_value(&opts->height, DEFAULT_HEIGHT);
	default_int_value(&opts->width, DEFAULT_WIDTH);

	if (opts->width == 0 || opts->height == 0) {
		fprintf(stderr,
				"argument error: height and width must be greater than 0\n");
		ret = -1;
	}

	if (opts->verbose)
		dump_opts(opts);
	global_opts = opts;
	return ret;
}

FILE *open_logfile(int rank) {
	char str[255];
	sprintf(str, "out-%d", rank);
	FILE *f = fopen(str, "w+");
	return f;
}

void init_grid(grid_t *grid, int start, int end, int rank) {
	grid_t g = *grid;
	int i, j;
	for (j = start; j < end; j++) {
		for (i = 0; i < g.width; i++) {
			int index = j * g.width + i;
			g.data[index] = rank;
		}
	}
}

ctx_t *make_ctx() {
	ctx_t *ctx = (ctx_t *) calloc(1, sizeof(ctx_t));
	return ctx;
}

void init_ctx(ctx_t *ctx, int width, int height) {
	int s;

	MPI_Comm_size(MPI_COMM_WORLD, &ctx->numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &ctx->rank);

	ctx->log = open_logfile(ctx->rank);
	ctx->dims[0] = ctx->numprocs;
	ctx->isperiodic[0] = 0;
	ctx->reorder = 0;

	/* allocate grid */
	ctx->grid = make_grid(width, height, 0);
	init_grid(ctx->grid, width, height, ctx->rank);

	/* compute start and end rows */
	decomp1d(height, ctx->numprocs, ctx->rank, &ctx->row_start, &ctx->row_end);

	/* create 1D cartesian communicator */
	MPI_Cart_create(MPI_COMM_WORLD, DIM_1D, ctx->dims, ctx->isperiodic, ctx->reorder, &ctx->comm1d);
	MPI_Cart_shift(ctx->comm1d, 0, 1, &ctx->north, &ctx->south);
	MPI_Cart_coords(ctx->comm1d, ctx->rank, DIM_1D, ctx->coords);

	/* prepare buffer for MPI_Bsend */
	MPI_Type_size(MPI_INTEGER, &s);
	ctx->buffer_size = s * width * 2 + s * MPI_BSEND_OVERHEAD * 2;
	ctx->buffer = (int *) malloc(ctx->buffer_size);
	MPI_Buffer_attach(ctx->buffer, ctx->buffer_size);
}

void dump_ctx(ctx_t *ctx) {
	fprintf(ctx->log, "*** CONTEXT ***\n");
	fprintf(ctx->log, "rank=%d dims[0]=%d coord[0]=%d\n", ctx->rank, ctx->dims[0], ctx->coords[0]);
	fprintf(ctx->log, "north=%d south=%d\n", ctx->north, ctx->south);
	fprintf(ctx->log, "row_start=%d row_end=%d\n", ctx->row_start, ctx->row_end);
	fprintf(ctx->log, "***************\n");
}

void free_ctx(ctx_t *ctx) {
	int x;
	free_grid(ctx->grid);
	MPI_Comm_free(&ctx->comm1d);
	MPI_Buffer_detach(&ctx->buffer, &x);
	FREE(ctx->buffer);
	FREE(ctx);
}

void exchng1d_async(ctx_t *ctx) {
	int width = ctx->grid->width;
	int *data = ctx->grid->data;
	int row_start = ctx->row_start;
	int row_end = ctx->row_end;
	int north = ctx->north;
	int south = ctx->south;
	MPI_Comm comm1d = ctx->comm1d;
	MPI_Request req[4];
	MPI_Status status[4];

	int *offset_recv_1 = data + (row_end - 1) * width;
	int *offset_recv_2 = data + row_start * width;
	MPI_Irecv(offset_recv_1, width, MPI_INTEGER, south, 0, comm1d, &req[1]);
	MPI_Irecv(offset_recv_2, width, MPI_INTEGER, north, 1, comm1d, &req[0]);

	int *offset_send_1 = data + (row_end - 2) * width;
	int *offset_send_2 = data + (row_start + 1) * width;
	MPI_Isend(offset_send_1, width, MPI_INTEGER, south, 0, comm1d, &req[3]);
	MPI_Isend(offset_send_2, width, MPI_INTEGER, north, 1, comm1d, &req[2]);

	MPI_Waitall(4, req, status);

	if (ctx->verbose) {
		fprintf(ctx->log, "after exchange south->north\n");
		fprint_matrix(data, width, row_start, row_end, ctx->log);
	}
}

void exchng1d_buffered(ctx_t *ctx) {
	MPI_Status status;

	int width = ctx->grid->width;
	int *data = ctx->grid->data;
	int row_start = ctx->row_start;
	int row_end = ctx->row_end;
	int north = ctx->north;
	int south = ctx->south;
	MPI_Comm comm1d = ctx->comm1d;

	int *offset_send = data + (row_end - 2) * width;
	int *offset_recv = data + row_start * width;
	MPI_Bsend(offset_send, width, MPI_INTEGER, south, 0, comm1d);
	MPI_Recv(offset_recv, width, MPI_INTEGER, north, 0, comm1d, &status);
	if (ctx->verbose) {
		fprintf(ctx->log, "after exchange north->south\n");
		fprint_matrix(data, width, row_start, row_end, ctx->log);
	}

	offset_send = data + (row_start + 1) * width;
	offset_recv = data + (row_end - 1) * width;
	MPI_Bsend(offset_send, width, MPI_INTEGER, north, 1, comm1d);
	MPI_Recv(offset_recv, width, MPI_INTEGER, south, 1, comm1d, &status);
	if (ctx->verbose) {
		fprintf(ctx->log, "after exchange south->north\n");
		fprint_matrix(data, width, row_start, row_end, ctx->log);
	}
}

void exchng1d_combined(ctx_t *ctx) {
	MPI_Status status;

	int width = ctx->grid->width;
	int *data = ctx->grid->data;
	int row_start = ctx->row_start;
	int row_end = ctx->row_end;
	int north = ctx->north;
	int south = ctx->south;
	MPI_Comm comm1d = ctx->comm1d;

	int *offset_send = data + (row_end - 2) * width;
	int *offset_recv = data + row_start * width;
	MPI_Sendrecv(offset_send, width, MPI_INTEGER, south, 0, offset_recv, width, MPI_INTEGER, north, 0, comm1d, &status);
	if (ctx->verbose) {
		fprintf(ctx->log, "after exchange north->south\n");
		fprint_matrix(data, width, row_start, row_end, ctx->log);
	}
	offset_send = data + (row_start + 1) * width;
	offset_recv = data + (row_end - 1) * width;
	MPI_Sendrecv(offset_send, width, MPI_INTEGER, north, 0, offset_recv, width, MPI_INTEGER, south, 0, comm1d, &status);
	if (ctx->verbose) {
		fprintf(ctx->log, "after exchange south->north\n");
		fprint_matrix(data, width, row_start, row_end, ctx->log);
	}
}

void exchng1d_ordered(ctx_t *ctx) {
	MPI_Status status;

	int width = ctx->grid->width;
	int *data = ctx->grid->data;
	int row_start = ctx->row_start;
	int row_end = ctx->row_end;
	int north = ctx->north;
	int south = ctx->south;
	MPI_Comm comm1d = ctx->comm1d;

	if ((ctx->coords[0] % 2) == 0) {
		int *offset_send = data + (row_end - 2) * width;
		int *offset_recv = data + row_start * width;
		MPI_Send(offset_send, width, MPI_INTEGER, south, 0, comm1d);
		MPI_Recv(offset_recv, width, MPI_INTEGER, north, 0, comm1d, &status);
		if (ctx->verbose) {
			fprintf(ctx->log, "after exchange north->south\n");
			fprint_matrix(data, width, row_start, row_end, ctx->log);
		}

		offset_send = data + (row_start + 1) * width;
		offset_recv = data + (row_end - 1) * width;
		MPI_Send(offset_send, width, MPI_INTEGER, north, 1, comm1d);
		MPI_Recv(offset_recv, width, MPI_INTEGER, south, 1, comm1d, &status);
		if (ctx->verbose) {
			fprintf(ctx->log, "after exchange south->north\n");
			fprint_matrix(data, width, row_start, row_end, ctx->log);
		}
	} else {
		int *offset_send = data + (row_end - 2) * width;
		int *offset_recv = data + row_start * width;
		MPI_Recv(offset_recv, width, MPI_INTEGER, north, 0, comm1d, &status);
		MPI_Send(offset_send, width, MPI_INTEGER, south, 0, comm1d);
		if (ctx->verbose) {
			fprintf(ctx->log, "after exchange north->south\n");
			fprint_matrix(data, width, row_start, row_end, ctx->log);
		}

		offset_send = data + (row_start + 1) * width;
		offset_recv = data + (row_end - 1) * width;
		MPI_Recv(offset_recv, width, MPI_INTEGER, south, 1, comm1d, &status);
		MPI_Send(offset_send, width, MPI_INTEGER, north, 1, comm1d);
		if (ctx->verbose) {
			fprintf(ctx->log, "after exchange south->north\n");
			fprint_matrix(data, width, row_start, row_end, ctx->log);
		}
	}
}

void exchng1d_blocking(ctx_t *ctx) {
	MPI_Status status;

	int width = ctx->grid->width;
	int *data = ctx->grid->data;
	int row_start = ctx->row_start;
	int row_end = ctx->row_end;
	int north = ctx->north;
	int south = ctx->south;
	MPI_Comm comm1d = ctx->comm1d;

	int *offset_send = data + (row_end - 2) * width;
	int *offset_recv = data + row_start * width;
	MPI_Send(offset_send, width, MPI_INTEGER, south, 0, comm1d);
	MPI_Recv(offset_recv, width, MPI_INTEGER, north, 0, comm1d, &status);
	if (ctx->verbose) {
		fprintf(ctx->log, "after exchange north->south\n");
		fprint_matrix(data, width, row_start, row_end, ctx->log);
	}

	offset_send = data + (row_start + 1) * width;
	offset_recv = data + (row_end - 1) * width;
	MPI_Send(offset_send, width, MPI_INTEGER, north, 1, comm1d);
	MPI_Recv(offset_recv, width, MPI_INTEGER, south, 1, comm1d, &status);
	if (ctx->verbose) {
		fprintf(ctx->log, "after exchange south->north\n");
		fprint_matrix(data, width, row_start, row_end, ctx->log);
	}

}

void hog(int x) {
	volatile double sum = 0.0;
	int i;
	for (i = 0; i < x; i++) {
		sum += (double) i / sum;
	}
}

int main(int argc, char **argv) {
	ctx_t *ctx = NULL;
	int rep;
	struct command_opts opts;

	if (parse_opts(argc, argv, &opts) < 0) {
		printf("Error while parsing arguments\n");
		usage();
	}

	handler exchng1d = opts.lib->handler;

	MPI_Init(&argc, &argv);

	ctx = make_ctx();
	init_ctx(ctx, opts.width, opts.height);
	ctx->verbose = opts.verbose;
	printf("rank=%d\n", ctx->rank);
	init_grid(ctx->grid, ctx->row_start, ctx->row_end, ctx->rank);
	if (ctx->verbose) {
		dump_ctx(ctx);
		fprintf(ctx->log, "initial data\n");
		fprint_matrix(ctx->grid->data, ctx->grid->width, ctx->row_start,
				ctx->row_end, ctx->log);
		exchng1d(ctx);
		ctx->verbose = 0;
	}

	for (rep = 0; rep < 10000; rep++) {
		exchng1d(ctx);
		//hog(1000);
	}

	free_ctx(ctx);
	MPI_Finalize();
	return EXIT_SUCCESS;
}
