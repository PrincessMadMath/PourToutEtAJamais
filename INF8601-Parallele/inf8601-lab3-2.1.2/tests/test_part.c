/*
 * test_part.c
 *
 *  Created on: 2011-11-08
 *      Author: francis
 */

#include <stdlib.h>
#include <stdio.h>

#include "part.h"
#include "memory.h"

void test_decomp1d()
{
    int nprocs, s, e, rank;
    int size = 100;
    //printf("%4s %4s %4s %4s %4s\n", "n", "size", "rank", "s", "e");
    for (nprocs=1; nprocs<=10; nprocs++) {
        for (rank=0; rank<nprocs; rank++) {
            decomp1d(size, nprocs, rank, &s, &e);
            //printf("%4d %4d %4d %4d %4d\n", nprocs, size, rank, s, e);
        }
    }
}

void test_part_sendcounts()
{
	int *array = NULL;
	int len, np, i;
	for(np=1; np<100; np++) {
		for(len=1; len<100; len++) {
			sendcounts_array(&array, np, len);
			int sum = 0;
			for (i=0; i<np; i++) {
				sum += array[i];
			}
			if (sum != len) {
				printf("sendcounts error np=%d len=%d sum=%d\n", np, len, sum);
				print_array(array, np);
			}
			FREE(array);
		}
	}
}

void test_part_displs()
{
    int *disp = NULL;
    int *sendcounts = NULL;
    int len, np, i;
    for(np=1; np<100; np++) {
        for(len=1; len<100; len++) {
            sendcounts_array(&sendcounts, np, len);
            displs_array(&disp, sendcounts, np);
            int sum = 0;
            for(i=0; i<np; i++) {
            	if (disp[i] != sum) {
            		printf("displs error np=%d len=%d disp=%d\n", np, len, disp[i]);
            		print_array(sendcounts, np);
            		print_array(disp, np);
            	}
            	sum += sendcounts[i];
            }
            FREE(disp);
            FREE(sendcounts);
        }
    }
}

int main(int argc, char **argv)
{
	test_part_sendcounts();
	test_part_displs();
    test_decomp1d();
	return 0;
}
