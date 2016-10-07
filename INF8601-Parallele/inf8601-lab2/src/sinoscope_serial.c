/*
 * sinoscope_serial.c
 *
 *  Created on: 2011-10-19
 *      Author: francis
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "color.h"
#include "sinoscope_serial.h"

int sinoscope_image_serial(sinoscope_t *ptr)
{
    if (ptr == NULL)
        return -1;

    sinoscope_t sino = *ptr;
    int x, y, index, taylor;
    struct rgb c;
    float val, px, py;

    x = 1;
    while(1) {
        y = 1;
        while(1) {
            px = sino.dx * y - 2 * M_PI;
            py = sino.dy * x - 2 * M_PI;
            val = 0.0f;
            for (taylor = 1; taylor <= sino.taylor; taylor += 2) {
                val += sin(px * taylor * sino.phase1 + sino.time) / taylor + cos(py * taylor * sino.phase0) / taylor;
            }
            val = (atan(1.0 * val) - atan(-1.0 * val)) / (M_PI);
            val = (val + 1) * 100;
            value_color(&c, val, sino.interval, sino.interval_inv);
            index = (y * 3) + (x * 3) * sino.width;
            sino.buf[index + 0] = c.r;
            sino.buf[index + 1] = c.g;
            sino.buf[index + 2] = c.b;
            y++;
            if (y >= sino.height-1)
                break;
        }
        x++;
        if (x >= sino.width-1)
            break;
    }
    return 0;
}
