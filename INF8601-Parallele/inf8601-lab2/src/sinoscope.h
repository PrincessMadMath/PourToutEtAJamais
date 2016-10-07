/*
 * sinoscope.h
 *
 *  Created on: 2011-10-13
 *      Author: francis
 */

#ifndef SINOSCOPE_H_
#define SINOSCOPE_H_

typedef struct sinoscope sinoscope_t;

struct sinoscope {
    unsigned char *buf;
    char *name;
    int buf_size;
    int width;
    int height;
    int interval;
    int taylor;
    float interval_inv;
    float time;
    float max;
    float phase0;
    float phase1;
    float dx;
    float dy;
};

sinoscope_t *make_sinoscope(int width, int height, int taylor, float max);
void free_sinoscope(sinoscope_t *b);
int init_data(int width, int height, int taylor);
int do_sinoscope(sinoscope_t *bilin);
void sinoscope_corners(sinoscope_t *b_ptr);
void run_gui(int argc, char **argv);

#endif /* SINOSCOPE_H_ */
