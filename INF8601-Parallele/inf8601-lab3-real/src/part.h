/*
 * part.h
 *
 *  Created on: 2011-11-08
 *      Author: francis
 */

#ifndef PART_H_
#define PART_H_

void decomp1d(int n, int size, int rank, int *s, int *e);
void sendcounts_array(int **array, int np, int len);
void displs_array(int **array, int *sendcounts, int np);
void print_array(int *array, int len);
void print_matrix(int *data, int width, int start, int end);
void fprint_matrix(int *data, int width, int start, int end, FILE *file);

#endif /* PART_H_ */
