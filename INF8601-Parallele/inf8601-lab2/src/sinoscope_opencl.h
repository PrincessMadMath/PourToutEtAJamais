/*
 * sinoscope_opencl.h
 *
 *  Created on: 2011-10-14
 *      Author: francis
 */

#ifndef SINOSCOPE_OPENCL_H_
#define SINOSCOPE_OPENCL_H_

#ifdef __cplusplus
extern "C" {
#endif

int sinoscope_image_opencl(sinoscope_t *ptr);
int opencl_init(int width, int height);
void opencl_shutdown();

#ifdef __cplusplus
}
#endif

#endif /* SINOSCOPE_OPENCL_H_ */
