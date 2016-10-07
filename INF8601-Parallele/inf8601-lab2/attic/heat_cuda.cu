#include <stdio.h>
#include <cuda.h>

#include "heat_cuda.h"

#define N 10
#define DIM 500

__global__ void kernel( void )
{
	return;
}

__global__ void add(int *a, int *b, int *c)
{
	int tid = blockIdx.x;
	if (tid < N)
		c[tid] = a[tid] + b[tid];
}

void cuda_devices()
{
	int count, i;
	cudaDeviceProp prop;

	cudaGetDeviceCount(&count);
	for(i=0; i<count; i++) {
		cudaGetDeviceProperties(&prop, i);
		printf("name=%s\n", prop.name);
		printf("mem (Mb)=%d\n", prop.totalGlobalMem/(1024*1024));
		printf("capability=%d.%d\n", prop.major, prop.minor);
		printf("clock (MHz)=%d\n", prop.clockRate / 1000);
		printf("proc count=%d\n", prop.multiProcessorCount);
	}
}

void cuda_add()
{
	int i;
	int a[N], b[N], c[N];
	int *dev_a, *dev_b, *dev_c;

	cudaMalloc((void**) &dev_a, N*sizeof(int));
	cudaMalloc((void**) &dev_b, N*sizeof(int));
	cudaMalloc((void**) &dev_c, N*sizeof(int));

	for (i=0; i < N; i++) {
		a[i] = -i;
		b[i] = i * i;
	}
	cudaMemcpy(dev_a, a, N*sizeof(int), cudaMemcpyHostToDevice);
	cudaMemcpy(dev_b, b, N*sizeof(int), cudaMemcpyHostToDevice);
	add<<<N,1>>>(dev_a, dev_b, dev_c);
	cudaMemcpy(c, dev_c, N*sizeof(int), cudaMemcpyDeviceToHost);
	for (i=0; i<N; i++) {
		printf("%d %d %d\n", a[i], b[i], c[i]);
	}

	cudaFree(dev_a);
	cudaFree(dev_b);
	cudaFree(dev_c);
}

struct cuComplex {
	float r;
	float i;
	__device__ cuComplex(float a, float b) : r(a), i(b) {}
	__device__ float magnitude2(void) { return r * r + i * i; }
	__device__ cuComplex operator*(const cuComplex& a) {
		return cuComplex(r*a.r - i*a.i, i*a.r + r*a.i);
	}
	__device__ cuComplex operator+(const cuComplex& a) {
		return cuComplex(r+a.r, i+a.i);
	}
};

__device__ int julia(int x, int y)
{
	int i;
	const float scale = 2.0;
	float jx = scale * (float) (DIM/2 - x) / (DIM/2);
	float jy = scale * (float) (DIM/2 - y) / (DIM/2);
	cuComplex c(-0.8, 0.156);
	cuComplex a(jx, jy);
	for (i=0; i<200; i++) {
		a = a * a + c;
		if (a.magnitude2() > 1000)
			return 0;
	}
	return 1;
}

__global__ void julia_kernel(unsigned char *ptr)
{
	int i = blockIdx.x;
	int j = blockIdx.y;
	int idx = i + j * gridDim.x;
	int juliaValue = julia(i, j);
	ptr[idx*3 + 0] = 255 * juliaValue;
	ptr[idx*3 + 1] = 0;
	ptr[idx*3 + 2] = 0;
}

int save_image(char *path, unsigned char *image, int width, int height)
{
	FILE *f = NULL;

    if (image == NULL || path == NULL)
    	return -1;

	if ((f = fopen(path, "wb")) == NULL) {
		char *msg;
		if (asprintf(&msg, "Failed to open %s", path) < 0)
				perror("Failed to open output file");
		else
				perror(msg);
		return -1;
	}

	fprintf(f, "P6\n%d %d\n%d\n", width, height, 255);
	fwrite(image, sizeof(char) * 3, width * height, f);
	fclose(f);
	return 0;
}

void julia_func()
{
	unsigned char *ptr = (unsigned char *) calloc(DIM*DIM*3, 1);

	unsigned char *dev_ptr;

	cudaMalloc((void**)&dev_ptr, DIM*DIM*3);
	dim3 grid(DIM, DIM);
	julia_kernel<<<grid,1>>>(dev_ptr);
	cudaMemcpy(ptr, dev_ptr, DIM*DIM*3, cudaMemcpyDeviceToHost);
	save_image("julia.ppm", ptr, DIM, DIM);
	cudaFree(dev_ptr);
}

void kernel_wrapper() {

}
