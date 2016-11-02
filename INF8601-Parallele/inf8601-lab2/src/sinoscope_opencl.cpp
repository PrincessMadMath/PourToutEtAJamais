/*
 * sinoscope_opencl.c
 *
 *  Created on: 2011-10-14
 *      Author: francis
 */

#include <iostream>

extern "C" {
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "sinoscope.h"
#include "color.h"
#include "memory.h"
#include "util.h"
}

#include "CL/opencl.h"
#include "sinoscope_opencl.h"

using namespace std;

#define BUF_SIZE 1024
#define COLOR_COUNT 3;
#define MAGIC "!@#~"
extern char __ocl_code_start, __ocl_code_end;
static cl_command_queue queue = NULL;
static cl_context context = NULL;
static cl_program prog = NULL;
static cl_kernel kernel = NULL;

static cl_mem output = NULL;
static cl_mem sino = NULL;

int get_opencl_queue()
{
    cl_int ret, i;
    cl_uint num_dev;
    cl_device_id device;
    cl_platform_id *platform_ids = NULL;
    cl_uint num_platforms;
    cl_uint info;
    char vendor[BUF_SIZE];
    char name[BUF_SIZE];

    ret = clGetPlatformIDs(0, NULL, &num_platforms);
    ERR_THROW(CL_SUCCESS, ret, "failed to get number of platforms");
    ERR_ASSERT(num_platforms > 0, "no opencl platform found");

    platform_ids = (cl_platform_id *) malloc(num_platforms * sizeof(cl_platform_id));
    ERR_NOMEM(platform_ids);

    ret = clGetPlatformIDs(num_platforms, platform_ids, NULL);
    ERR_THROW(CL_SUCCESS, ret, "failed to get plateform ids");

    for (i = 0; i < num_platforms; i++) {
        ret = clGetPlatformInfo(platform_ids[i], CL_PLATFORM_VENDOR, BUF_SIZE, vendor, NULL);
        ERR_THROW(CL_SUCCESS, ret, "failed to get plateform info");
        ret = clGetPlatformInfo(platform_ids[i], CL_PLATFORM_NAME, BUF_SIZE, name, NULL);
        ERR_THROW(CL_SUCCESS, ret, "failed to get plateform info");
        cout << vendor << " " << name << "\n";
        ret = clGetDeviceIDs(platform_ids[0], CL_DEVICE_TYPE_GPU, 1, &device, &num_dev);
        if (CL_SUCCESS == ret) {
            break;
        }
        ret = clGetDeviceIDs(platform_ids[0], CL_DEVICE_TYPE_CPU, 1, &device, &num_dev);
        if (CL_SUCCESS == ret) {
            break;
        }
    }
    ERR_THROW(CL_SUCCESS, ret, "failed to find a device\n");

    ret = clGetDeviceInfo(device, CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(cl_uint), &info, NULL);
    ERR_THROW(CL_SUCCESS, ret, "failed to get device info");
    cout << "clock frequency " << info << "\n";

    ret = clGetDeviceInfo(device, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(cl_uint), &info, NULL);
    ERR_THROW(CL_SUCCESS, ret, "failed to get device info");
    cout << "compute units " << info << "\n";

    context = clCreateContext(0, 1, &device, NULL, NULL, &ret);
    ERR_THROW(CL_SUCCESS, ret, "failed to create context");

    queue = clCreateCommandQueue(context, device, 0, &ret);
    ERR_THROW(CL_SUCCESS, ret, "failed to create queue");

    ret = 0;

done:
    FREE(platform_ids);
    return ret;
error:
    ret = -1;
    goto done;
}

int load_kernel_code(char **code, size_t *length)
{
    int ret = 0;
    int found = 0;
    size_t kernel_size = 0;
    size_t code_size = 0;
    char *kernel_code = NULL;
    char *ptr = NULL;
    char *code_start = NULL, *code_end = NULL;

    code_start = &__ocl_code_start;
    code_end = &__ocl_code_end;
    code_size = code_end - code_start;
    kernel_code = (char *) malloc(code_size);
    ERR_NOMEM(kernel_code);
    for (ptr = code_start; ptr < code_end - 4; ptr++) {
        if (ptr[0] == '!' && ptr[1] == '@' && ptr[2] == '#' && ptr[3] == '~') {
            found = 1;
            break;
        }
    }
    if (found == 0)
        goto error;
    kernel_size = ptr - code_start;
    strncpy(kernel_code, code_start, kernel_size);
    *code = kernel_code;
    *length = kernel_size;
done:
    return ret;
error:
    ret = -1;
    FREE(kernel_code);
    goto done;
}

int create_buffer(int width, int height)
{
    cl_int ret;
	cl_int errorCode;
	
	// Initialise mémoire requise pour matrices de l'image
	int requiredSize = width * height * sizeof(char) * COLOR_COUNT;
	output = clCreateBuffer(context, CL_MEM_WRITE_ONLY,requiredSize, NULL, &errorCode); 
	ERR_THROW(CL_SUCCESS,ret,"Error with 'CreateBuffer' for output");

	// Initialise mémoire requise pour le sinoscope
	sino = clCreateBuffer(context,CL_MEM_READ_ONLY ,sizeof(sinoscope_t), NULL, &errorCode);
	ERR_THROW(CL_SUCCESS,ret,"Error with 'CreateBuffer' for sino");
	
    ret = 0;
    
done:
    return ret;
error:
    ret = -1;
    goto done;
}

int opencl_init(int width, int height)
{
    cl_int err;
    char *code = NULL;
    size_t length = 0;

    get_opencl_queue();
    if (queue == NULL)
        return -1;
    load_kernel_code(&code, &length);
    if (code == NULL)
        return -1;

    /*
     * Initialisation du programme
     */
    prog = clCreateProgramWithSource(context, 1, (const char **) &code, &length, &err);
    ERR_THROW(CL_SUCCESS, err, "clCreateProgramWithSource failed");
    err = clBuildProgram(prog, 0, NULL, "-cl-fast-relaxed-math", NULL, NULL);
    ERR_THROW(CL_SUCCESS, err, "clBuildProgram failed");
    kernel = clCreateKernel(prog, "sinoscope_kernel", &err);
    ERR_THROW(CL_SUCCESS, err, "clCreateKernel failed");
    err = create_buffer(width, height);
    ERR_THROW(CL_SUCCESS, err, "create_buffer failed");

    free(code);
    return 0;
error:
    return -1;
}


void opencl_shutdown()
{
    if (queue) 	clReleaseCommandQueue(queue);
    if (context)	clReleaseContext(context);

    if(prog) clReleaseProgram(prog);
    if(kernel) clReleaseKernel(kernel);
    if(output) clReleaseMemObject(output);
}

int sinoscope_image_opencl(sinoscope_t *ptr)
{
	cl_int ret = 0;
    cl_event ev;
    
	int requiredSize;
	size_t work_dimension[2] =  {(size_t) ptr->width, (size_t) ptr->height};;
	
    if (ptr == NULL)
        goto error;
	

    
     //   1. Passer les arguments au noyau avec clSetKernelArg(). Si des
     //     arguments sont passees par un tampon, copier les valeurs avec
     //     clEnqueueWriteBuffer() de maniere synchrone.
    
    // Arg0 = output
    ret = clSetKernelArg(kernel,0,sizeof(cl_mem),&output);
	ERR_THROW(CL_SUCCESS,ret,"Error with 'clSetKernelArg'");
	
	// Arg1 = sino
	ret = clSetKernelArg(kernel,1,sizeof(cl_mem),&sino);
	ERR_THROW(CL_SUCCESS,ret,"Set kernel argument ptr failed");
     
    // Copie de l'object sinoscope (de l'hote) au GPU
    ret = clEnqueueWriteBuffer(queue,sino,CL_TRUE,0,sizeof(sinoscope_t),ptr,0,NULL,NULL);
    ERR_THROW(CL_SUCCESS,ret,"Error with 'EnqueueWriteBuffer'");

        
    // 2. Enqueu range	
	ret = clEnqueueNDRangeKernel(queue, kernel, 2, 0, work_dimension, NULL, 0, NULL, NULL);
	ERR_THROW(CL_SUCCESS,ret,"Error with 'EnqueueNDRangeKernel'");

	// 3. Attendre que le noyau termine avec clFinish()
	ret = clFinish(queue);
	ERR_THROW(CL_SUCCESS,ret,"Error with 'Finish'");	
	
	// 4. Lire les résultat : copie dans output
	requiredSize = ptr->width * ptr->height * sizeof(char) * COLOR_COUNT;
	ret = clEnqueueReadBuffer(queue,output,CL_TRUE,0,requiredSize,ptr->buf,0,NULL,NULL);
	ERR_THROW(CL_SUCCESS,ret,"Error with 'EnqueueReadBuffer'");	

done:
    return ret;
error:
    ret = -1;
	goto done;
}
