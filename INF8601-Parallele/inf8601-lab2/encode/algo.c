/*
 * algo.c
 *
 *  Created on: 2011-09-25
 *      Author: francis
 */

#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>

#include "algo.h"
#include "chunk.h"
#include "omp.h"

int sigma(int n)
{
    return (n + 1) * n;
}

struct cs {
    uint64_t checksum;
} __attribute__((aligned(64)));

int encode_fast(struct chunk *chunk)
{
    // TODO
    chunk->checksum = 42;
    return 0;
}

int encode_slow_a(struct chunk *chunk)
{
    int i, j;
    uint64_t checksum = 0;

    #pragma omp parallel for private(i,j) reduction(+:checksum)
    for (i = 0; i < chunk->height; i++) {
        for (j = 0; j < chunk->width; j++) {
            int index = i * chunk->width + j;
            chunk->data[index] = chunk->data[index] + chunk->key;
            checksum += chunk->data[index];
        }
    }
    chunk->checksum = checksum;
    return 0;
}

int encode_slow_b(struct chunk *chunk)
{
    int i;
    int area = chunk->area;
    int key = chunk->key;
    char *data = chunk->data;
    uint64_t* checksums;
    int n;

    #pragma omp parallel private(i)
    {
        #pragma omp single
        {
            n = omp_get_num_threads();
            checksums = calloc(n, sizeof(uint64_t));
        }
        #pragma omp barrier
        int id = omp_get_thread_num();
        for (i = id; i < area; i += n) {
            data[i] = data[i] + key;
            checksums[id] += data[i];
        }
        #pragma omp barrier
    }
    chunk->checksum = 0;
    for (i = 0; i < n; i++)
        chunk->checksum += checksums[i];
    return 0;
}

int encode_slow_c(struct chunk *chunk)
{
    int i;
    int checksum = 0;
    char *data = chunk->data;
    int area = chunk->area;
    int key = chunk->key;

    #pragma omp parallel for
    for (i = 0; i < area; i++) {
        data[i] = data[i] + key;
        #pragma omp atomic
        checksum += data[i];
    }
    chunk->checksum = checksum;
    return 0;
}

int encode_slow_d(struct chunk *chunk)
{
    int i;
    int checksum = 0;
    char *data = chunk->data;
    int area = chunk->area;
    int key = chunk->key;

    #pragma omp parallel for
    for (i = 0; i < area; i++) {
        data[i] = data[i] + key;
        #pragma omp critical
        {
            checksum += data[i];
        }
    }
    chunk->checksum = checksum;
    return 0;

}

int encode_slow_e(struct chunk *chunk)
{

    int i, j;
    int checksum = 0;
    int width = chunk->width;
    int height = chunk->height;
    int key = chunk->key;
    char *data = chunk->data;

    #pragma omp parallel for private(i,j) reduction(+:checksum)
    for (i = 0; i < width; i++) {
        for (j = 0; j < height; j++) {
            int index = i + j * width;
            data[index] = data[index] + key;
            checksum += data[index];
        }
    }
    chunk->checksum = checksum;
    return 0;
}

int encode_slow_f(struct chunk *chunk)
{
    int i;
    int area = chunk->area;
    int key = chunk->key;
    char *data = chunk->data;
    struct cs* cs;
    int n;
    int sig;
    uint64_t checksum;

    #pragma omp parallel private(i, checksum)
    {
        #pragma omp single
        {
            n = omp_get_num_threads();
            cs = calloc(n, sizeof(struct cs));
            sig = sigma(n);
        }
        #pragma omp barrier
        checksum = 0;
        int id = omp_get_thread_num();
        int start = (int) (((uint64_t)sigma(id)) * area / sig);
        int end   = (int) (((uint64_t)sigma(id + 1)) * area / sig);

        for (i = start; i < end; i++) {
            data[i] = data[i] + key;
            checksum += data[i];
        }
        #pragma omp barrier
        cs[id].checksum = checksum;
    }
    chunk->checksum = 0;
    for (i = 0; i < n; i++)
        chunk->checksum += cs[i].checksum;
    return 0;
}

