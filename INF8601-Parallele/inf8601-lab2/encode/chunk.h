/*
 * chunk.h
 *
 *  Created on: 2011-09-17
 *      Author: francis
 */

#ifndef CHUNK_H_
#define CHUNK_H_

#include <stdint.h>

struct chunk {
    char *data;
    int width;
    int height;
    int area;
    char key;
    uint64_t checksum;
};

struct chunk *make_chunk(int width, int height);
void free_chunk(struct chunk *m);
void randomize_chunk(struct chunk *chunk);
void linear_chunk(struct chunk *chunk);
void dump_chunk(struct chunk *chunk);
size_t chunk_size(struct chunk *chunk);

#endif /* CHUNK_H_ */
