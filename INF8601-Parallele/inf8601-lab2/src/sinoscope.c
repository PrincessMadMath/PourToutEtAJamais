/*
 * sinoscope.c
 *
 *  Created on: 2011-09-25
 *      Author: francis
 */

#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <math.h>
#include <time.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <omp.h>
#include <sys/types.h>
#include <unistd.h>
#include <inttypes.h>
#include <GL/glew.h>
#include <GL/glxew.h>
#include <GL/glut.h>

#include "config.h"
#include "sinoscope.h"
#include "sinoscope_openmp.h"
#include "sinoscope_opencl.h"
#include "sinoscope_serial.h"
#include "color.h"
#include "memory.h"
#include "util.h"

/* Globals and defaults */
#define PROGNAME "sinoscope"
#define DEFAULT_HEIGHT	512
#define DEFAULT_WIDTH 	512
#define DEFAULT_LIB_NAME "serial"
#define DEFAULT_CMD_NAME "gui"
#define DEFAULT_IMG_PATH "sinoscope.ppm"
#define DEFAULT_TAYLOR 3
#define DEFAULT_ITER 10
#define TITLE "inf8601-lab2"
#define FPS_DELAY 3000
#define BYTE_PER_PIX 3
#define MICROSECONDS 1000000

static int win_x, win_y, win_id;
static sinoscope_t *global_bl = NULL;
static GLuint tex = 0;
static int enable_display = 1;
static struct timeval fpsStart;
static long fpsCount = 0;
static float max = 1000000.0;
static float amp = 200.0;
static const struct command_def const *commands[];

struct stats {
	struct timeval elapsed;
	struct timeval user;
	struct timeval system;
};

enum thread_lib {
	LIB_NONE,
	LIB_SERIAL,
	LIB_OPENMP,
	LIB_OPENCL,
};

struct command_opts {
	const struct command_def *cmd;
	const struct lib_def *lib;
	char *ppm_path;
	int enable_output;
	int height;
	int width;
	int taylor;
	int iter;
	int verbose;
};

typedef int (*sinoscope_handler)(sinoscope_t *);

struct lib_def {
	const char *name;
	enum thread_lib type;
	sinoscope_handler handler;
};

static struct command_opts *global_opts = NULL;

static const struct lib_def libs[] = {
		{ .name = "serial", .type = LIB_SERIAL, .handler = sinoscope_image_serial },
		{ .name = "openmp", .type = LIB_OPENMP, .handler = sinoscope_image_openmp },
		{ .name = "opencl", .type = LIB_OPENCL, .handler = sinoscope_image_opencl },
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
	fprintf(stderr, "  --cmd		command [ gui | benchmark | image ]\n");
	fprintf(stderr, "  --lib		set the threading library to use "\
			"[ serial | openmp | opencl ]\n");
	fprintf(stderr, "  --output set image path output\n");
	fprintf(stderr, "  --height	set height\n");
	fprintf(stderr, "  --width	set width\n");
	fprintf(stderr, "  --taylor	set taylor series terms\n");
	fprintf(stderr, "  --iter 	set number of benchmark iterations\n");
	fprintf(stderr, "\n");
	exit(EXIT_FAILURE);
}

static int init_lib(struct command_opts *opts)
{
	int ret = 0;
	if (opts == NULL)
		return -1;

	switch (opts->lib->type) {
	case LIB_SERIAL:
	case LIB_OPENMP:
		break;
	case LIB_OPENCL:
		ret = opencl_init(opts->width, opts->height);
		ERR_THROW(0, ret, "init_data error");
	default:
		break;
	}
	return 0;
	error:
	return -1;
}

static int close_lib(struct command_opts *opts)
{
	if (opts == NULL)
		return -1;

	switch (opts->lib->type) {
	case LIB_SERIAL:
	case LIB_OPENMP:
		break;
	case LIB_OPENCL:
		opencl_shutdown();
	default:
		break;
	}
	return 0;
}

static int cmd_gui(struct command_opts *opts)
{
	int ret = 0;

	ret = init_data(opts->width, opts->height, opts->taylor);
	ERR_THROW(0, ret, "init_data error");

	init_lib(opts);
	ERR_THROW(0, ret, "init_lib error");

	run_gui(0, NULL);
	close_lib(opts);

	error:
	return ret;
}

struct timeval time_sub(struct timeval t1, struct timeval t2)
{
	struct timeval res;
	res.tv_sec  = t1.tv_sec  - t2.tv_sec;
	res.tv_usec = t1.tv_usec - t2.tv_usec;
	if(t1.tv_usec < t2.tv_usec) {
		res.tv_sec--;
		res.tv_usec += MICROSECONDS;
	}
	return res;
}

void write_stats_header(FILE *f)
{
	fprintf(f, "experiment,width,height,iter,u,s,e\n");
}

void write_stats_info(FILE *f, char *name, int width, int height, int iter)
{
	fprintf(f, "%s,%d,%d,%d,", name, width, height, iter);
}

void write_stats(FILE *f, struct stats *s)
{
	if (s == NULL)
		return;
	fprintf(f, "%ld.%06ld,%ld.%06ld,%ld.%06ld\n",
			s->user.tv_sec, s->user.tv_usec,
			s->system.tv_sec, s->system.tv_usec,
			s->elapsed.tv_sec, s->elapsed.tv_usec);
}

int run_benchmark(struct stats *s, sinoscope_t *sinoscope, sinoscope_handler handler, int iter)
{
	int ret = 0;
	int i;
	struct rusage r1, r2;
	struct timeval t1, t2;

	gettimeofday(&t1, NULL);
	if (getrusage(RUSAGE_SELF, &r1) < 0) {
		perror("getrusage failed");
		goto err;
	}

	for(i = 0; i < iter; i++) {
		printf("%-10s %3.0f %%\r", sinoscope->name, (100 * i) / (float) iter);
		fflush(stdout);
		handler(sinoscope);
	}
	printf("%-10s %3.0f %%\n", sinoscope->name, 100.0);

	if (getrusage(RUSAGE_SELF, &r2) < 0) {
		perror("getrusage failed");
		goto err;
	}
	gettimeofday(&t2, NULL);

	if (s != NULL) {
		s->user    = time_sub(r2.ru_utime, r1.ru_utime);
		s->system  = time_sub(r2.ru_stime, r1.ru_stime);
		s->elapsed = time_sub(t2, t1);
	}

done:
	return ret;
err:
	ret = -1;
	goto done;
}

static int cmd_benchmark(struct command_opts *opts)
{
	int ret = 0;
	sinoscope_t *b = NULL;
	struct stats stats;
	char fname[256];
	sprintf(fname, "sinoscope-%d.out", getpid());
	FILE* f = fopen(fname, "w");

	write_stats_header(f);

	b = make_sinoscope(opts->width, opts->height, opts->taylor, amp);
	ERR_NOMEM(b);

	/* serial */
	b->name = "serial";
	write_stats_info(f, b->name, opts->width, opts->height, opts->iter);
	run_benchmark(&stats, b, sinoscope_image_serial, opts->iter);
	write_stats(f, &stats);

	/* openmp 1 thread */
	b->name = "openmp_1";
	omp_set_num_threads(1);
	write_stats_info(f, b->name, opts->width, opts->height, opts->iter);
	run_benchmark(&stats, b, sinoscope_image_openmp, opts->iter);
	write_stats(f, &stats);

	/* openmp 8 threads */
	b->name = "openmp_8";
	omp_set_num_threads(8);
	write_stats_info(f, b->name, opts->width, opts->height, opts->iter);
	run_benchmark(&stats, b, sinoscope_image_openmp, opts->iter);
	write_stats(f, &stats);

	/* opencl */
	b->name = "opencl";
	ret = opencl_init(opts->width, opts->height);
	ERR_THROW(0, ret, "opencl_init failed");
	write_stats_info(f, b->name, opts->width, opts->height, opts->iter);
	run_benchmark(&stats, b, sinoscope_image_opencl, opts->iter);
	write_stats(f, &stats);
	opencl_shutdown();
	ERR_THROW(0, ret, "opencl_shutdown failed");
	printf("\n");
	fprintf(f, "\n");
	free_sinoscope(b);
	b = NULL;
done:
	free_sinoscope(b);
	if (f != NULL)
		fclose(f);
	return ret;
error:
	ret = -1;
	goto done;
}

static int cmd_image(struct command_opts *opts)
{
	int ret;
	sinoscope_t *s = NULL;

	ret = init_lib(opts);
	ERR_THROW(0, ret, "init_lib error");

	s = make_sinoscope(opts->width, opts->height, opts->taylor, amp);
	ERR_NOMEM(s);
	ret = opts->lib->handler(s);
	ERR_THROW(0, ret, "handler returned error");
	ret = save_image_uchar(opts->ppm_path, s->buf, s->width, s->height);
	ERR_THROW(0, ret, "save image failed");
done:
	close_lib(opts);
	free_sinoscope(s);
	return ret;
error:
	ret = -1;
	goto done;
}

static const struct command_def cmd_gui_def =
{ .name = "gui", .handler = cmd_gui };
static const struct command_def cmd_benchmark_def =
{ .name = "benchmark", .handler = cmd_benchmark };
static const struct command_def cmd_image_def =
{ .name = "image", .handler = cmd_image };
static const struct command_def cmd_def_last =
{ .name = NULL, .handler = NULL };

static const struct command_def const *commands[] = {
		&cmd_gui_def,
		&cmd_benchmark_def,
		&cmd_image_def,
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
	printf("%10s %s\n", "output", opts->ppm_path);
	printf("%10s %d\n", "width", opts->width);
	printf("%10s %d\n", "height", opts->height);
	printf("%10s %d\n", "taylor", opts->taylor);
	printf("%10s %d\n", "iter", opts->iter);
}

void default_int_value(int *val, int def)
{
	if (*val == 0)
		*val = def;
}

static int parse_opts(int argc, char **argv, struct command_opts *opts)
{
	int idx;
	int opt;
	int ret = 0;

	struct option options[] = {
			{ "help",	 0, 0, 'h' },
			{ "cmd",	 1, 0, 'c' },
			{ "output",	 1, 0, 'o' },
			{ "lib",	 1, 0, 'l' },
			{ "height",	 1, 0, 'y' },
			{ "width",	 1, 0, 'x' },
			{ "taylor",	 1, 0, 't' },
			{ "iter",	 1, 0, 'i' },
			{ "verbose", 0, 0, 'v' },
			{ 0, 0, 0, 0}
	};

	memset(opts, 0, sizeof(struct command_opts));
	opts->height = DEFAULT_HEIGHT;
	opts->width = DEFAULT_WIDTH;
	opts->taylor = DEFAULT_TAYLOR;
	opts->iter = DEFAULT_ITER;

	while ((opt = getopt_long(argc, argv, "hvx:y:c:l:o:t:i:", options, &idx)) != -1) {
		switch(opt) {
		case 'c':
			opts->cmd = lookup_cmd(optarg);
			break;
		case 'l':
			opts->lib = lookup_lib(optarg);
			if (opts->lib == NULL) {
				printf("unknown threading lib %s\n", optarg);
				ret = -1;
			}
			break;
		case 'o':
			if (asprintf(&opts->ppm_path, "%s", optarg) < 0)
				goto err;
			break;
		case 'y':
			opts->height = atoi(optarg);
			break;
		case 'x':
			opts->width = atoi(optarg);
			break;
		case 't':
			opts->taylor = atoi(optarg);
			break;
		case 'i':
			opts->iter = atoi(optarg);
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

	if (opts->cmd == NULL)
		opts->cmd = lookup_cmd(DEFAULT_CMD_NAME);

	if (opts->ppm_path == NULL)
		opts->ppm_path = DEFAULT_IMG_PATH;

	if (opts->width == 0 || opts->height == 0) {
		fprintf(stderr, "argument error: height and width must be greater than 0\n");
		ret = -1;
	}

	if (opts->verbose)
		dump_opts(opts);
	global_opts = opts;

done:
	return ret;
err:
	ret = -1;
	goto done;
}

sinoscope_t *make_sinoscope(int width, int height, int taylor, float max)
{
	sinoscope_t *b = calloc(1, sizeof(sinoscope_t));
	if (b == NULL)
		return NULL;
	b->buf_size = width * height * BYTE_PER_PIX;
	b->buf = malloc(b->buf_size);
	if (b->buf == NULL)
		return NULL;
	b->width = width;
	b->height = height;
	b->max = max;
	b->interval = get_color_interval(max);
	b->interval_inv = get_color_interval_inv(max);
	b->taylor = taylor;
	b->dx = 3 * M_PI / width;
	b->dy = 3 * M_PI / height;
	return b;
}

void free_sinoscope(sinoscope_t *b)
{
	if (b != NULL)
		FREE(b->buf);
	FREE(b);
}

int init_data(int width, int height, int taylor)
{
	win_x = width;
	win_y = height;
	if (global_bl == NULL) {
		global_bl = make_sinoscope(width, height, taylor, amp);
	}
	if (global_bl == NULL)
		return -1;
	return 0;
}

void free_data()
{
	free_sinoscope(global_bl);
}

void sinoscope_corners(sinoscope_t *b_ptr)
{
	if (b_ptr == NULL)
		return;
	if (b_ptr->width == 0 || b_ptr->height == 0)
		return;
	float time = b_ptr->time;
	float fact = (b_ptr->width * b_ptr->height) * 0.5;
	b_ptr->phase0 = ((max / 2) * sinf(time * 0.1) + (max / 2)) / fact;
	b_ptr->phase1 = ((max / 2) * sinf(time * 0.2) + (max / 2)) / fact;

	b_ptr->time += 0.01;
	if (b_ptr->time > 2*M_PI*1000) {
		b_ptr->time -= 2*M_PI*1000;
	}
}

void fps_update(int value)
{
	char fps[256];
	struct timeval t;
	gettimeofday(&t, NULL);
	struct timeval diff = time_sub(t, fpsStart);
	double delay = diff.tv_sec + ((double) diff.tv_usec / 1000000.0);
	sprintf(fps, TITLE " %s (%d x %d): %.1f fps", global_opts->lib->name, win_x, win_y, fpsCount / delay);
	glutSetWindowTitle(fps);
	printf("%s\n", fps);
	glutTimerFunc(FPS_DELAY, fps_update, value);
}

void draw_sinoscope(const struct lib_def *def, sinoscope_t *b)
{
	int ret = 0;
	if (def == NULL || b == NULL) {
		printf("BUG in draw_sinoscope\n");
		exit(1);
	}
	sinoscope_corners(b);
	ret = def->handler(b);
	if (ret < 0) {
		printf("Error while executing sinoscope %s\n", def->name);
		exit(1);
	}
	if (tex == 0) {
		glGenTextures(1, &tex);
	}
	if (enable_display) {
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, b->width, b->height, 0, GL_RGB, GL_UNSIGNED_BYTE, b->buf);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glEnable(GL_TEXTURE_2D);
		glBegin(GL_QUADS);
		glTexCoord2d(0.0,0.0); glVertex2d(0.0,0.0);
		glTexCoord2d(1.0,0.0); glVertex2d(1.0,0.0);
		glTexCoord2d(1.0,1.0); glVertex2d(1.0,1.0);
		glTexCoord2d(0.0,1.0); glVertex2d(0.0,1.0);
		glEnd();
	}
	fpsCount++;
}

void pre_display()
{
	glViewport(0, 0, win_x, win_y);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, 1.0, 0.0, 1.0);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void post_display()
{
	glutSwapBuffers();
}

void init_fps()
{
	fpsCount = 0;
	gettimeofday(&fpsStart, NULL);
}

void key_func(unsigned char key, int x, int y)
{
	init_fps();
	switch (key) {
	case 'q':
	case 'Q':
		free_data();
		exit(0);
		break;
	case '1':
		close_lib(global_opts);
		global_opts->lib = lookup_lib("serial");
		init_lib(global_opts);
		break;
	case '2':
		close_lib(global_opts);
		global_opts->lib = lookup_lib("openmp");
		init_lib(global_opts);
		break;
	case '3':
		close_lib(global_opts);
		global_opts->lib = lookup_lib("opencl");
		init_lib(global_opts);
		break;
	case ' ':
		enable_display = !enable_display;
		break;
	}
}

static void mouse_func(int button, int state, int x, int y)
{
}

static void motion_func(int x, int y)
{
}

static void reshape_func(int width, int height)
{
	glutSetWindow(win_id);
	glutReshapeWindow(width, height);

	win_x = width;
	win_y = height;
}

static void idle_func()
{
	glutSetWindow(win_id);
	glutPostRedisplay();
}

static void display_func()
{
	pre_display();
	draw_sinoscope(global_opts->lib, global_bl);
	post_display();
}

void open_glut_window()
{
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);

	int posx = (glutGet(GLUT_SCREEN_WIDTH) - win_x) / 2;
	int posy = (glutGet(GLUT_SCREEN_HEIGHT) - win_y) / 2;
	glutInitWindowPosition(posx, posy);
	glutInitWindowSize(win_x, win_y);
	win_id = glutCreateWindow(TITLE);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glutSwapBuffers();
	glClear(GL_COLOR_BUFFER_BIT);
	glutSwapBuffers();

	pre_display();

	glutKeyboardFunc(key_func);
	glutMouseFunc(mouse_func);
	glutMotionFunc(motion_func);
	glutReshapeFunc(reshape_func);
	glutIdleFunc(idle_func);
	glutDisplayFunc(display_func);
	glutTimerFunc(FPS_DELAY, fps_update, 0);

	glewInit();
	glXSwapIntervalSGI(0);

	// May cause segfault
	//glXSwapIntervalMESA(0);
	//glXSwapIntervalEXT(glXGetCurrentDisplay(), glXGetCurrentDrawable(), 0);
}

void run_gui(int argc, char **argv)
{
	init_fps();
	glutInit( &argc, argv );
	open_glut_window();
	glutMainLoop();
}

int main(int argc, char **argv)
{
	srand48(time(NULL));
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
