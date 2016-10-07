/*
 * heat_cuda.h
 *
 *  Created on: 2011-10-13
 *      Author: francis
 */

#ifndef HEAT_CUDA_H_
#define HEAT_CUDA_H_

#ifdef __cplusplus
extern "C" {
#endif
void kernel_wrapper(void);
void cuda_devices();
void cuda_add();
void julia_func();
#ifdef __cplusplus
}
#endif

#endif /* HEAT_CUDA_H_ */
