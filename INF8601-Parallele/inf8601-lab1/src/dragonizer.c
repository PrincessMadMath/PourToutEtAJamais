/*
 * dragonizer.c
 *
 * Programme dessinant la fractale du dragon en utilisant soit pthread ou TBB
 *
 *  Created on: 2011-06-26
 *	  Author: francis
 */

#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <error.h>
#include <getopt.h>
#include <inttypes.h>

#include "config.h"
#include "dragon.h"
#include "dragon_pthread.h"
#include "dragon_tbb.h"

/* Globals and defaults */
#define PROGNAME "dragonizer"
#define DEFAULT_HEIGHT	512
#define DEFAULT_WIDTH 	512
#define DEFAULT_SIZE	1048576
#define DEFAULT_NB_THREAD 2
#define DEFAULT_LIB_NAME "serial"
#define DEFAULT_IMG_PATH "dragon.ppm"
#define POWER_MAX 		30
#define CHECK_POWER 	20
#define CHECK_NB_THREAD	8
static const struct command_def const *commands[];
int verbose = 0;

/*
 * Over POWER_MAX = 30, the types used in array indexes overflows
 * and memory usage is very high, limiting power to 2^29
 * */

enum thread_lib {
	THREAD_LIB_NONE,
	THREAD_LIB_SERIAL,
	THREAD_LIB_PTHREAD,
	THREAD_LIB_TBB,
};

struct command_opts {
	const struct command_def *cmd;
	const struct lib_def *lib;
	char *pgm_path;
	int nb_thread;
	int height;
	int width;
	int power;
	int power_max;
	int verbose;
	uint64_t size;
};

typedef int (*draw_handler)(char **, struct rgb *, int, int, uint64_t, int);
typedef int (*limits_handler)(limits_t *, uint64_t, int);

struct lib_def {
	const char *name;
	enum thread_lib lib;
	draw_handler draw_handler;
	limits_handler limits_handler;
};

static const struct lib_def libs[] = {
		{ .name = "serial",
				.lib = THREAD_LIB_SERIAL,
				.draw_handler = dragon_draw_serial,
				.limits_handler = dragon_limits_serial },
		{ .name = "pthread",
				.lib = THREAD_LIB_PTHREAD,
				.draw_handler = dragon_draw_pthread,
				.limits_handler = dragon_limits_pthread },
		{ .name = "tbb",
				.lib = THREAD_LIB_TBB,
				.draw_handler = dragon_draw_tbb,
				.limits_handler = dragon_limits_tbb },
		{ .name = NULL,
				.lib = THREAD_LIB_NONE,
				.draw_handler = NULL,
				.limits_handler = NULL },
};

typedef int (*cmd_handler)(struct command_opts*);

struct command_def {
	const char 			*name;
	cmd_handler 		handler;
};

__attribute__((noreturn))
static void usage(void)
{
	fprintf(stderr, PROGNAME " " VERSION " " PACKAGE_NAME "\n");
	fprintf(stderr, "Usage: " PROGNAME " [OPTIONS] [COMMAND]\n");
	fprintf(stderr, "\nOptions:\n");
	fprintf(stderr, "  --help	this help\n");
	fprintf(stderr, "  --cmd		command [ draw | limits | check ]\n");
	fprintf(stderr, "  --thread	set number of threads\n");
	fprintf(stderr, "  --lib		set the threading library to use "\
			"[ serial | pthread | tbb ]\n");
	fprintf(stderr, "  --output set image path output\n");
	fprintf(stderr, "  --height	set dragon height\n");
	fprintf(stderr, "  --width	set dragon width\n");
	fprintf(stderr, "  --size	set dragon size\n");
	fprintf(stderr, "  --power  set dragon size by power\n");
	fprintf(stderr, "  --max    compute all dragon to max power\n");
	fprintf(stderr, "\n");
	exit(EXIT_FAILURE);
}

static int cmd_draw(struct command_opts *opts)
{
	char *dragon = NULL;
	struct rgb *img;
	int ret = 0;

	img = make_canvas(opts->width, opts->height);
	if (img == NULL)
		goto err;

	switch (opts->lib->lib) {
	case THREAD_LIB_SERIAL:
	case THREAD_LIB_PTHREAD:
	case THREAD_LIB_TBB:
		if (opts->power > 0 && opts->power_max > 0) {
			int i;
			for (i = opts->power; i <= opts->power_max; i++) {
				uint64_t size = 1LL << i;
				if (opts->verbose)
					printf("draw size=%"PRId64"\n", size);
				ret = opts->lib->draw_handler(&dragon, img, opts->width, opts->height,
						size, opts->nb_thread);
				if (i != opts->power_max)
					FREE(dragon);
				if (ret < 0)
					break;
			}
		} else {
			if (opts->verbose)
				printf("draw size=%"PRId64"\n", opts->size);
			ret = opts->lib->draw_handler(&dragon, img, opts->width, opts->height, opts->size,
				opts->nb_thread);
		}
		break;
	case THREAD_LIB_NONE:
	default:
		ret = -1;
		break;
	}
	if (ret < 0)
		goto err;

	write_img(img, opts->pgm_path, opts->width, opts->height);
done:
	FREE(dragon);
	FREE(img);
	return ret;
err:
	ret = -1;
	goto done;
}

static const struct command_def cmd_draw_def =
{ .name = "draw", .handler = cmd_draw };

static int cmd_limits(struct command_opts *opts)
{
	int ret = 0;
	limits_t limits;
	memset(&limits, 0, sizeof(limits_t));

	switch (opts->lib->lib) {
	case THREAD_LIB_SERIAL:
	case THREAD_LIB_PTHREAD:
	case THREAD_LIB_TBB:
		if (opts->power > 0 && opts->power_max > 0) {
			int i;
			for (i = opts->power; i <= opts->power_max; i++) {
				uint64_t size = 1LL << i;
				if (opts->verbose)
					printf("limits size=%"PRId64"\n", size);
				ret = opts->lib->limits_handler(&limits, size, opts->nb_thread);
				if (ret < 0)
					break;
			}
		} else {
			if (opts->verbose)
				printf("limits size=%"PRId64"\n", opts->size);
			ret = opts->lib->limits_handler(&limits, opts->size, opts->nb_thread);
		}
		break;
	case THREAD_LIB_NONE:
	default:
		ret = -1;
	}

	if (ret < 0)
		goto err;

	dump_limits(&limits);
done:
	return ret;
err:
	ret = -1;
	goto done;
}

static const struct command_def cmd_limit_def =
{ .name = "limits", .handler = cmd_limits };

static int check_limits(struct command_opts *opts)
{
	int ret = 0;
	int i;
	limits_t lim_expected, lim_actual;
	memset(&lim_expected, 0, sizeof(limits_t));

	if (dragon_limits_serial(&lim_expected, opts->size, opts->nb_thread) < 0) {
		printf("Error: limits serial failed\n");
		return -1;
	}

	for (i = 1; libs[i].lib != THREAD_LIB_NONE; i++) {
		memset(&lim_actual, 0, sizeof(limits_t));
		const char *name = libs[i].name;
		ret = libs[i].limits_handler(&lim_actual, opts->size, opts->nb_thread);
		if (ret < 0) {
			printf("Error executing limits with %s\n", name);
			return -1;
		}
		if (cmp_limits(&lim_expected, &lim_actual) == 0) {
			printf("PASS %10s %10s\n", "limits", name);
		} else {
			ret = -1;
			printf("FAIL %10s %10s\n", "limits", name);
			printf("expected: "); dump_limits(&lim_expected);
			printf("actual  : "); dump_limits(&lim_actual);
		}
	}
	return ret;
}

static int check_draw(struct command_opts *opts)
{
	int ret = 0;
	int errors = 0;
	int i;
	limits_t limits;
	int area;
	int dragon_width;
	int dragon_height;
	int threshold;
	char *drg_exp = NULL, *drg_act = NULL;
	struct rgb *img_exp = NULL, *img_act = NULL;
	char *f1 = NULL, *f2 = NULL;

	uint64_t min_size = 1LL << CHECK_POWER;
	if (opts->size < min_size && opts->nb_thread < CHECK_NB_THREAD)
		printf("For best results, check with power at " \
				"least %d and thread at least %d\n", CHECK_POWER, CHECK_NB_THREAD);

	if (dragon_limits_serial(&limits, opts->size, opts->nb_thread) < 0) {
		printf("Error: limits serial failed\n");
		goto err;
	}

	dragon_width = limits.maximums.x - limits.minimums.x;
	dragon_height = limits.maximums.y - limits.minimums.y;
	area = dragon_width * dragon_height;
	threshold = opts->nb_thread * 2;

	img_exp = make_canvas(opts->width, opts->height);
	img_act = make_canvas(opts->width, opts->height);
	if (img_exp == NULL || img_act == NULL)
		goto err;

	if (dragon_draw_serial(&drg_exp, img_exp, opts->width, opts->height, opts->size, opts->nb_thread) < 0) {
		printf("Error: draw serial failed\n");
		goto err;
	}

	char *fmt = "%s %10s %10s threshold=%d gap=%d (%.3f%%)\n";
	for (i = 1; libs[i].lib != THREAD_LIB_NONE; i++) {
		const char *name = libs[i].name;
		ret = libs[i].draw_handler(&drg_act, img_act, opts->width, opts->height, opts->size, opts->nb_thread);
		if (ret < 0) {
			printf("Error executing draw with %s\n", name);
			goto err;
		}
		int gap = cmp_canvas(drg_exp, drg_act, dragon_width, dragon_height, opts->verbose);
		float gap_f = gap * 100 / ((float) area);
		if (gap < threshold && gap >= 0) {
			printf(fmt, "PASS", "draw", name, threshold, gap, gap_f);
		} else {
			errors++;
			printf(fmt, "FAIL", "draw", name, threshold, gap, gap_f);
			if (asprintf(&f1, "dragon_check_failed_serial.ppm") < 0)
				goto err;
			if (asprintf(&f2, "dragon_check_failed_%s.ppm", name) < 0)
				goto err;
			if (write_img(img_exp, f1, opts->width, opts->height) < 0)
				goto err;
			if (write_img(img_act, f2, opts->width, opts->height) < 0)
				goto err;
			printf("expected: %s\n", f1);
			printf("actual  : %s\n", f2);
			FREE(f1);
			FREE(f2);
		}
		FREE(drg_act);
	}

done:
	FREE(img_exp);
	FREE(img_act);
	FREE(drg_exp);
	FREE(drg_act);
	FREE(f1);
	FREE(f2);
	if (errors != 0)
	    ret = -1;
	return ret;
err:
	ret = -1;
	goto done;
}

static int cmd_check(struct command_opts *opts)
{
	int ret = 0;
	if (check_limits(opts) < 0)
		ret = -1;
	if (check_draw(opts) < 0)
		ret = -1;
	return ret;
}

static const struct command_def cmd_check_def =
{ .name = "check", .handler = cmd_check };

static const struct command_def cmd_def_last =
{ .name = NULL, .handler = NULL };

static const struct command_def const *commands[] = {
		&cmd_draw_def,
		&cmd_limit_def,
		&cmd_check_def,
		&cmd_def_last
};

static const struct command_def *lookup_cmd(const char *name)
{
	int i;
	for (i = 0; commands[i]->name != NULL; i++) {
		if (strcmp(commands[i]->name, name) == 0)
			return commands[i];
	}
	return NULL;
}

static const struct lib_def *lookup_lib(const char *name)
{
	int i;
	for (i = 0; libs[i].name != NULL; i++) {
		if (strcmp(libs[i].name, name) == 0)
			return &libs[i];
	}
	return NULL;
}

static void dump_opts(struct command_opts *opts)
{
	printf("%10s %s\n", "option", "value");
	printf("%10s %s\n", "cmd", opts->cmd->name);
	printf("%10s %s\n", "lib", opts->lib->name);
	printf("%10s %s\n", "output", opts->pgm_path);
	printf("%10s %d\n", "thread", opts->nb_thread);
	printf("%10s %d\n", "height", opts->height);
	printf("%10s %d\n", "width", opts->width);
	printf("%10s %" PRId64 "\n", "size", opts->size);
	printf("%10s %d\n", "power", opts->power);
	printf("%10s %d\n", "max", opts->power_max);
}

void default_int_value(int *value, int def)
{
	if (*value == 0)
		*value = def;
}

static int parse_opts(int argc, char **argv, struct command_opts *opts)
{
	int idx;
	int opt;
	int ret = 0;

	struct option options[] = {
			{ "help",	 0, 0, 'h' },
			{ "cmd",	 1, 0, 'c' },
			{ "thread",	 1, 0, 't' },
			{ "output",	 1, 0, 'o' },
			{ "lib",	 1, 0, 'l' },
			{ "height",	 1, 0, 'y' },
			{ "width",	 1, 0, 'x' },
			{ "size",	 1, 0, 's' },
			{ "power",	 1, 0, 'p' },
			{ "max",	 1, 0, 'm' },
			{ "verbose", 0, 0, 'v' },
			{ 0, 0, 0, 0}
	};

	memset(opts, 0, sizeof(struct command_opts));

	while ((opt = getopt_long(argc, argv, "hvx:y:s:c:t:l:p:o:m:", options, &idx)) != -1) {
		switch(opt) {
		case 'c':
			opts->cmd = lookup_cmd(optarg);
			break;
		case 't':
			opts->nb_thread = atoi(optarg);
			break;
		case 'l':
			opts->lib = lookup_lib(optarg);
			if (opts->lib == NULL) {
				printf("unknown threading lib %s\n", optarg);
				ret = -1;
			}
			break;
		case 'o':
			if (asprintf(&opts->pgm_path, "%s", optarg) < 0)
				goto err;
			break;
		case 'y':
			opts->height = atoi(optarg);
			break;
		case 'x':
			opts->width = atoi(optarg);
			break;
		case 's':
			opts->size = atoi(optarg);
			break;
		case 'p':
			opts->power = atoi(optarg);
			break;
		case 'm':
			opts->power_max = atoi(optarg);
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

	if (opts->pgm_path == NULL)
		opts->pgm_path = DEFAULT_IMG_PATH;

	if (opts->size > (1LL << POWER_MAX)) {
		printf("Error: size must be lower or equals to %"PRId64"\n", opts->size);
		ret = -1;
	}
	if ((opts->power < 0) || (opts->power >= POWER_MAX)) {
		printf("Error: power argument out of range [0,%d]\n", POWER_MAX);
		ret = -1;
	}

	if (opts->power_max < 0 || opts->power >= POWER_MAX) {
		printf("Error: max argument out of range [0,%d]\n", POWER_MAX);
		ret = -1;
	}

	if (opts->power > 0 && opts->power_max > 0) {
		if (opts->power > opts->power_max) {
			printf("Error: max must be greater than or equals to power\n");
			ret = -1;
		}
	}

	if (opts->power > 0)
		opts->size = 1LL << opts->power;

	if (opts->size ==  0)
		opts->size = DEFAULT_SIZE;

	default_int_value(&opts->height, DEFAULT_HEIGHT);
	default_int_value(&opts->width, DEFAULT_WIDTH);
	default_int_value(&opts->nb_thread, DEFAULT_NB_THREAD);

	if (opts->width == 0 || opts->height == 0) {
		fprintf(stderr, "argument error: height and width must be greater than 0\n");
		ret = -1;
	}

	if (opts->verbose)
		dump_opts(opts);

done:
	return ret;
err:
	ret = -1;
	goto done;
}

int main(int argc, char **argv)
{
	struct command_opts opts;
	if (parse_opts(argc, argv, &opts) < 0) {
		printf("Error while parsing arguments\n");
		usage();
	}

	if (opts.cmd == NULL) {
		printf("Select a command to run\n");
		usage();
	}

	if ((opts.cmd->handler(&opts)) < 0) {
		printf("Error while executing command %s\n", opts.cmd->name);
		goto err;
	}

	return EXIT_SUCCESS;

	err:
	exit(EXIT_FAILURE);
}

