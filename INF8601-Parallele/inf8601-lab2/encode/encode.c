/*
 * encode.c
 *
 *  Created on: 2011-09-18
 *      Author: francis
 */

#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <getopt.h>
#include <inttypes.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <errno.h>
#include <unistd.h>
#include <sched.h>
#include <sys/syscall.h>

#include "chunk.h"
#include "algo.h"
#include "omp.h"
#include "config.h"

#define PROGNAME "encode"
#define DEFAULT_OUTPUT "stats.csv"
#define DEFAULT_HEIGHT	512
#define DEFAULT_WIDTH 	512
#define DEFAULT_REPEAT	1
#define DEFAULT_NB_THREAD 2
#define DEFAULT_HYPERTHREAD  1
#define DEFAULT_FUNC "fast"
#define DEFAULT_CMD "check"
#define ONE_MB 1048576
#define MICROSECONDS_PER_SECOND 1000000
int verbose = 0;

static const struct command_def const *commands[];

struct command_opts;
typedef int (*cmd_handler)(struct command_opts*);

struct command_def {
    const char 			*name;
    cmd_handler 		handler;
};

struct command_opts {
    const struct command_def *cmd;
    const struct encoder_def *enc;
    int nb_thread;
    int height;
    int width;
    int verbose;
    int repeat;
    int max;
    int hyperthread;
    char *output;
};

struct stats {
    struct timeval elapsed;
    struct timeval user;
    struct timeval system;
};

__attribute__((noreturn))
static void usage(void)
{
    fprintf(stderr, PROGNAME " " VERSION " " PACKAGE_NAME "\n");
    fprintf(stderr, "Usage: " PROGNAME " [OPTIONS] [COMMAND]\n");
    fprintf(stderr, "\nOptions:\n");
    fprintf(stderr, "  --help               this help\n");
    fprintf(stderr, "  --cmd                command [ check | benchmark ]\n");
    fprintf(stderr, "  --thread             set number of threads\n");
    fprintf(stderr, "  --max                set max number of threads for benchmark\n");
    fprintf(stderr, "  --height             set buffer height\n");
    fprintf(stderr, "  --width              set buffer width\n");
    fprintf(stderr, "  --repeat             set number of roundtrip processing\n");
    fprintf(stderr, "  --hyperthread        set hyperthreading (0 disable, 1 force, default 1)\n");
    fprintf(stderr, "  --func               only execute this function\n");
    fprintf(stderr, "  --output             set output file (default: %s)\n", DEFAULT_OUTPUT);
    fprintf(stderr, "\n");
    exit(EXIT_FAILURE);
}

static const struct encoder_def encoders[] = {
        { .name = "fast", .encode_handler = encode_fast },
        { .name = "slow_a", .encode_handler = encode_slow_a },
        { .name = "slow_b", .encode_handler = encode_slow_b },
        { .name = "slow_c", .encode_handler = encode_slow_c },
        { .name = "slow_d", .encode_handler = encode_slow_d },
        { .name = "slow_e", .encode_handler = encode_slow_e },
        { .name = "slow_f", .encode_handler = encode_slow_f },
        { .name = NULL, .encode_handler = NULL }
};

int gettid() {
    return (int) syscall(SYS_gettid);
}

int init_openmp(struct command_opts *opts) {
    omp_set_num_threads(opts->nb_thread);
    int error = 0;
    #pragma omp parallel reduction(+:error)
    {
        cpu_set_t cpuset;
        int cpus = sysconf(_SC_NPROCESSORS_ONLN);
        int id = omp_get_thread_num();
        int core = id % cpus;
        if (opts->hyperthread)
            core = (id * 4) % cpus;
        CPU_ZERO(&cpuset);
        CPU_SET(core, &cpuset);
        sched_setaffinity(gettid(), sizeof(cpuset), &cpuset);
        if (opts->verbose)
            printf("pin %d %d %d\n", gettid(), id, core);
        if (core != sched_getcpu())
            error++;
    }
    return !!error;
}

int encode_dummy(struct command_opts *opts) {
    int i;

    #pragma omp parallel for
    for (i = 0; i < opts->nb_thread * 10; i++) {
        if (opts->verbose)
            printf("omp id=%d tid=%d cpu=%d %d\n", omp_get_thread_num(),
                gettid(), sched_getcpu(), i);
    }
    return 0;
}

int self_check(struct command_opts *opts)
{
    (void) opts;
    int i, ret;
    uint64_t exp, act;
    struct chunk *chunk = make_chunk(DEFAULT_WIDTH, DEFAULT_HEIGHT);
    if (chunk == NULL)
        return -1;

    chunk->key = 42;

    for (i = 0; encoders[i].name != NULL; i++) {
        linear_chunk(chunk);
        exp = chunk->checksum;
        encoders[i].encode_handler(chunk);
        chunk->key = -chunk->key;
        encoders[i].encode_handler(chunk);
        chunk->key = -chunk->key;
        act = chunk->checksum;
        if (act != exp) {
            printf("FAIL %s exp=%"PRId64" act=%"PRId64"\n", encoders[i].name, exp, act);
        } else {
            printf("PASS %s\n", encoders[i].name);
        }
    }
    free_chunk(chunk);

    ret = encode_dummy(opts);
    opts->hyperthread = !opts->hyperthread;
    init_openmp(opts);
    ret |= encode_dummy(opts);
    printf("%s hyperthread\n", ret == 0 ? "PASS" : "FAIL");
    return 0;
}

int roundtrip(struct chunk *chunk, encode_fct encode)
{
    // encode
    encode(chunk);
    // decode
    chunk->key = -chunk->key;
    encode(chunk);
    // set the key to the original value
    chunk->key = -chunk->key;
    return 0;
}

struct timeval time_sub(struct timeval t1, struct timeval t2)
{
    struct timeval res;
    res.tv_sec  = t1.tv_sec  - t2.tv_sec;
    res.tv_usec = t1.tv_usec - t2.tv_usec;
    if(t1.tv_usec < t2.tv_usec) {
        res.tv_sec--;
        res.tv_usec += MICROSECONDS_PER_SECOND;
    }
    return res;
}

void write_stats_header(FILE *f, struct command_opts *opts, struct chunk *chunk)
{
    if (opts == NULL || f == NULL)
        return;
    double size = ((double)chunk_size(chunk) * opts->repeat * 2) / ONE_MB;
    fprintf(f, "EXPERIMENT thread=%d width=%d height=%d repeat=%d hyperthread=%d size(Mib)=%.3f\n",
            opts->nb_thread, opts->width, opts->height, opts->repeat, opts->hyperthread, size);
    fprintf(f, "%s\n", "func,u,s,e");
}

void write_stats(FILE *f, struct stats *s)
{
    if (s == NULL)
        return;
    fprintf(f, "%ld.%06ld,%ld.%06ld,%ld.%06ld,",
            s->user.tv_sec, s->user.tv_usec,
            s->system.tv_sec, s->system.tv_usec,
            s->elapsed.tv_sec, s->elapsed.tv_usec);
}

int run_benchmark(struct stats *s, struct chunk *chunk, encode_fct encoder, int iter)
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
        roundtrip(chunk, encoder);
    }

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

static int cmd_check(struct command_opts *opts)
{
    self_check(opts);
    return 0;
}

static int do_benchmark(struct chunk *chunk, const struct encoder_def *enc,
        int thread, int repeat, FILE *out)
{
    struct stats stats;
    fprintf(stderr, "processing %-6s %2d threads\n", enc->name, thread);
    fprintf(out, "%s,", enc->name);
    int ret = run_benchmark(&stats, chunk, enc->encode_handler, repeat);
    if (ret < 0)
        return -1;
    write_stats(out, &stats);
    fprintf(out, "\n");
    return 0;
}

static int cmd_benchmark(struct command_opts *opts)
{
    struct chunk *chunk = NULL;
    int i, t;
    int ret = 0;

    FILE *f = fopen(opts->output, "w");
    if (f == NULL)
        goto err;
    ret = ftruncate(fileno(f), 0);
    if (ret < 0)
        goto err;

    chunk = make_chunk(opts->width, opts->height);
    if (chunk == NULL)
        goto err;

    chunk->key = 13;
    randomize_chunk(chunk);

    write_stats_header(f, opts, chunk);
    for(t = opts->nb_thread; t <= opts->max; t++) {
        if (opts->enc == NULL) {
            for (i = 0; encoders[i].name != NULL; i++) {
                do_benchmark(chunk, &encoders[i], t, opts->repeat, f);
            }
        } else { /* perform only for one encoder */
            do_benchmark(chunk, opts->enc, t, opts->repeat, f);
        }
        fprintf(f, "\n");
    }

done:
    free_chunk(chunk);
    if (f != NULL)
        fclose(f);
    return ret;
err:
    ret = -1;
    goto done;
}

static const struct command_def cmd_check_def =
{ .name = "check", .handler = cmd_check };

static const struct command_def cmd_benchmark_def =
{ .name = "benchmark", .handler = cmd_benchmark };

static const struct command_def cmd_def_last =
{ .name = NULL, .handler = NULL };

static const struct command_def const *commands[] = {
        &cmd_benchmark_def,
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

static const struct encoder_def *lookup_func(const char *name)
{
    int i;
    for (i = 0; encoders[i].name != NULL; i++) {
        if (strcmp(encoders[i].name, name) == 0)
            return &encoders[i];
    }
    return NULL;
}

static void dump_opts(struct command_opts *opts)
{
    printf("%10s %s\n", "option", "value");
    printf("%10s %s\n", "cmd", opts->cmd->name);
    printf("%10s %d\n", "thread", opts->nb_thread);
    printf("%10s %d\n", "height", opts->height);
    printf("%10s %d\n", "width", opts->width);
    printf("%10s %d\n", "size", opts->repeat);
    printf("%10s %d\n", "max", opts->max);
    printf("%10s %d\n", "hyperthread", opts->hyperthread);
    printf("%10s %s\n", "output", opts->output);
    if (opts->enc != NULL)
        printf("%10s %s\n", "func", opts->enc->name);
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
            { "thread",	 1, 0, 't' },
            { "height",	 1, 0, 'y' },
            { "width",	 1, 0, 'x' },
            { "repeat",	 1, 0, 'r' },
            { "max",	 1, 0, 'm' },
            { "func",	 1, 0, 'f' },
            { "hyperthread", 1, 0, 'n' },
            { "output",  1, 0, 'o' },
            { "verbose", 0, 0, 'v' },
            { 0, 0, 0, 0}
    };

    memset(opts, 0, sizeof(struct command_opts));
    /* default values*/
    opts->hyperthread = DEFAULT_HYPERTHREAD;
    opts->repeat =  DEFAULT_REPEAT;
    opts->height = DEFAULT_HEIGHT;
    opts->width = DEFAULT_WIDTH;
    opts->nb_thread = DEFAULT_NB_THREAD;
    opts->cmd = lookup_cmd(DEFAULT_CMD);
    opts->enc = NULL;

    while ((opt = getopt_long(argc, argv, "hvn:x:y:r:c:t:m:f:o:", options, &idx)) != -1) {
        switch(opt) {
        case 'c':
            opts->cmd = lookup_cmd(optarg);
            break;
        case 't':
            opts->nb_thread = atoi(optarg);
            break;
        case 'y':
            opts->height = atoi(optarg);
            break;
        case 'x':
            opts->width = atoi(optarg);
            break;
        case 'r':
            opts->repeat = atoi(optarg);
            break;
        case 'm':
            opts->max = atoi(optarg);
            break;
        case 'f':
            opts->enc = lookup_func(optarg);
            break;
        case 'n':
            opts->hyperthread = atoi(optarg);
            break;
        case 'o':
            opts->output = strdup(optarg);
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

    if (opts->max < opts->nb_thread)
        opts->max = opts->nb_thread;

    if (opts->width == 0 || opts->height == 0) {
        fprintf(stderr, "argument error: height and width must be greater than 0\n");
        ret = -1;
    }
    if (opts->hyperthread != 0 && opts->hyperthread != 1) {
        fprintf(stderr, "argument error: hyperthread must be 0 or 1\n");
        ret = -1;
    }

    if (opts->output == NULL)
        opts->output = strdup(DEFAULT_OUTPUT);

    if (opts->cmd == NULL) {
        printf("Select a command to run\n");
        ret = -1;
    }

    if (opts->verbose || ret < 0)
        dump_opts(opts);

    if (ret == -1)
        goto err;
done:
    return ret;
err:
    free(opts->output);
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

    init_openmp(&opts);
    fprintf(stderr, "running %s\n", opts.cmd->name);
    if ((opts.cmd->handler(&opts)) < 0) {
        printf("Error while executing command %s\n", opts.cmd->name);
        goto err;
    }
    fprintf(stderr, "done\n");
    free(opts.output);
    return EXIT_SUCCESS;

err:
    exit(EXIT_FAILURE);
}
