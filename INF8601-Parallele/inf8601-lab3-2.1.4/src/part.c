/*
 * part.c
 *
 *  Created on: 2011-11-08
 *      Author: francis
 */

#include <stdlib.h>
#include <stdio.h>

#include "part.h"
#include "memory.h"

void decomp1d(int size, int nprocs, int rank, int *s, int *e)
{
    *s = size * rank / nprocs;
    *e = (size * (rank + 1)) / nprocs;
}

void print_array(int *array, int len)
{
	int i;
	for (i = 0; i < len; i++)
		printf("%d ", array[i]);
	printf("\n");
}

void print_matrix(int *data, int width, int start, int end)
{
    int i, j;
    for (j=start; j<end; j++) {
        for (i=0; i<width; i++) {
            int index = j * width + i;
            printf("%d ", data[index]);
        }
        printf("\n");
    }
}

void fprint_matrix(int *data, int width, int start, int end, FILE *file)
{
    int i, j;
    for (j=start; j<end; j++) {
        for (i=0; i<width; i++) {
            int index = j * width + i;
            fprintf(file, "%d ", data[index]);
        }
        fprintf(file, "\n");
    }
}


void sendcounts_array(int **array, int np, int len)
{
	if (array == NULL || np == 0 || len == 0)
		return;
	int i;
	int *a = (int *) malloc(sizeof(int) * np);
	int base = len / np;
	int mod = len % np;
	for (i = 0; i < np; i++) {
		int size = base;
		if (mod > 0) {
			size++;
			mod--;
		}
		a[i] = size;
	}
	*array = a;
}

void displs_array(int **array, int *sendcounts, int np)
{
	int i;
	int disp = 0;
	int *a = (int *) malloc(sizeof(int) * np);

	for (i = 0; i < np; i++) {
		a[i] = disp;
		disp += sendcounts[i];
	}
	*array = a;
}
