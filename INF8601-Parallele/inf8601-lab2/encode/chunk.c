/*
 * chunk.c
 *
 *  Created on: 2011-09-17
 *      Author: francis
 */

#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <time.h>

#include "chunk.h"
#include "omp.h"

struct chunk *make_chunk(int width, int height)
{
    char *data;
    struct chunk *m = calloc(1, sizeof(struct chunk));
    if (m == NULL)
        return NULL;
    m->width = width;
    m->height = height;
    m->area = width * height;
    data = malloc(m->area * sizeof(char));
    if (data == NULL) {
        free(m);
        return NULL;
    }
    m->data = data;
    return m;
}

void free_chunk(struct chunk *m)
{
    if(m == NULL)
        return;
    if (m->data != NULL) {
        free(m->data);
        m->data = NULL;
    }
    free(m);
    m = NULL;
    return;
}

void randomize_chunk(struct chunk *chunk)
{
    int i;
    struct chunk c = *chunk;
    c.checksum = 0;
    srand(time(NULL));
    for (i = 0; i < c.area; i++) {
        c.data[i] = rand();
        c.checksum += c.data[i];
    }
    *chunk = c;
}

void linear_chunk(struct chunk *chunk)
{
    int i;
    struct chunk c = *chunk;
    c.checksum = 0;
    for (i = 0; i<c.area; i++) {
        c.data[i] = i % 255;
        c.checksum += c.data[i];
    }
    *chunk = c;
}

void dump_chunk(struct chunk *chunk)
{
    int i, j;
    struct chunk c = *chunk;
    for (i = 0; i<c.height; i++) {
        for (j = 0; j<c.width; j++) {
            printf("%c ", c.data[i * c.width + j]);
        }
        printf("\n");
    }
    printf("checksum=%"PRId64"\n", c.checksum);
}

size_t chunk_size(struct chunk *chunk)
{
    return sizeof(uint64_t) * chunk->area;
}
