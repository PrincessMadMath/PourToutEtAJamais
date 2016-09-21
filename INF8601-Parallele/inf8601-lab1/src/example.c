/*
 * example.c
 *
 *  Created on: 2011-08-08
 *      Author: francis
 */

/*
 * Test values from dragon functions
 */

#include <stdio.h>
#include <inttypes.h>

#include "dragon.h"
#include "color.h"

void test_compute_position_orientation()
{
	int64_t i;
	printf("position orientation\n");
	printf("%5s %5s %5s %5s %5s\n", "i", "p.x", "p.y", "o.x", "o.y");
	for(i=0; i<20; i++) {
		xy_t p = compute_position(i);
		xy_t o = compute_orientation(i);
		printf("%5d %5d %5d %5d %5d\n", (int)i,
				(int)p.x, (int)p.y, (int)o.x, (int)o.y);
	}
}

void test_compute_limits()
{
	int64_t i;
	printf("limit\n");
	printf("%5s %5s %5s %5s %5s\n", "i", "max.x", "max.y", "min.x", "min.y");
	for(i=0; i<20; i++) {
		limits_t l;
		dragon_limits_serial(&l, i, 0);
		printf("%5d %5d %5d %5d %5d\n", (int)i,
				(int)l.maximums.x, (int)l.maximums.y,
				(int)l.minimums.x, (int)l.minimums.y);

	}
}

void test_compute_limits_set()
{
	printf("limit set\n");
	printf("%5s %5s %5s %5s %5s %5s\n", "start", "end", "max.x", "max.y", "min.x", "min.y");

	piece_t piece1, piece2, piece3, piece4;

	piece_init(&piece1);
	piece_limit(0, 100, &piece1);
	dump_limits(&piece1.limits);

	piece_init(&piece2);
	piece_limit(100, 200, &piece2);
	dump_limits(&piece2.limits);

	piece_init(&piece3);
	piece_limit(0, 200, &piece3);
	dump_limits(&piece3.limits);

	piece_init(&piece4);
	piece_merge(&piece4, piece1);
	dump_limits(&piece4.limits);
	piece_merge(&piece4, piece2);
	dump_limits(&piece4.limits);
}

int check_piece(piece_t *p1, piece_t *p2)
{
	return !(p1->limits.maximums.x == p2->limits.maximums.x &&
		p1->limits.maximums.y == p2->limits.maximums.y &&
		p1->limits.minimums.x == p2->limits.minimums.x &&
		p1->limits.minimums.y == p2->limits.minimums.y);

	/*&&
		p1->orientation.x == p2->orientation.x &&
		p1->orientation.y == p2->orientation.y &&
		p1->position.x == p2->position.x &&
		p1->position.x == p2->position.x);
		*/
}

void test_check_limits()
{
	int i;
	int j;
	int max_thread;
	int rand;

	for (max_thread = 1; max_thread <= 8; max_thread++) {
		for (rand = 0; rand < 100; rand++) {
			for (i = 1; i < 22; i++) {
				uint64_t size = (1LL << i) + rand;
				piece_t master;
				piece_init(&master);
				for (j = 0; j < max_thread; j++) {
					piece_t piece;
					piece_init(&piece);
					uint64_t start = j * size / max_thread;
					uint64_t end = (j + 1) * size / max_thread;
					piece_limit(start, end, &piece);
					piece_merge(&master, piece);
				}
				piece_t serial;
				piece_init(&serial);
				piece_limit(0, size, &serial);
				//dump_limits(&serial.limits);
				//dump_limits(&master.limits);
				if (check_piece(&serial, &master) != 0) {
					printf("FAIL size=%d max_threads=%d\n", i, j);
					break;
				} else {
				//	printf("PASS size=%d max_threads=%d\n", i, j);
				}
			}
		}
	}
}

void test_rotate()
{
	int i;
	xy_t o;
	o.x = 1;
	o.y = 1;
	printf("rotate_left\n");
	printf("%5s %5s\n", "o.x", "o.y");
	printf("%5d %5d\n", (int)o.x, (int)o.y);
	for(i = 0; i<4; i++) {
		rotate_left(&o);
		printf("%5d %5d\n", (int)o.x, (int)o.y);
	}
}

void test_random_color()
{
	int i;
	int max = 10;
	struct rgb color;
	for (i = 0; i < max; i++) {
		random_color(&color);
		printf("%d %d %d\n", color.r, color.g, color.b);
	}
}

void test_init_palette()
{
	int num = 8;
	int width = 500;
	int height = 500;
	int max = 255;
	struct rgb *img;
	int x, y, r;

	img = (struct rgb *) malloc(sizeof(struct rgb) * width * height);
	if (img == NULL)
		return;
	struct palette *palette = init_palette(num);
	if (palette == NULL)
		return;
	dump_palette(palette);

	int row_height = height / num;
	for (r = 0; r < num; r++)
		for (y = r * row_height; y < (r + 1) * row_height; y++)
			for (x = 0; x < width; x++)
				img[y * width + x] = palette->colors[r];

	printf("width=%d height=%d\n", width, height);
	FILE *f = fopen("test.ppm", "w+");
	fprintf(f, "P6\n%d %d\n%d\n", width, height, max);
	fwrite(img, sizeof(struct rgb), width * height, f);
	free(img);

	free_palette(palette);
}

void nicolas_hill(int size, int nb_thread)
{
	int thread_number;
	for (nb_thread=1; nb_thread<=10; nb_thread++) {
		printf("nb_thread=%d\n", nb_thread);
		int prev = 0;
		for(thread_number = 0; thread_number < nb_thread; thread_number++) {
			int blockSize = (int)(size/nb_thread);
			int startingPos = 0;
			if(thread_number < size%blockSize) {
				blockSize++;
				startingPos = thread_number * blockSize;
			} else {
				startingPos = size%blockSize*(blockSize+1);
				startingPos += blockSize*(thread_number - size%blockSize);
			}
			if (prev != startingPos)
				printf("ERROR ");
			printf("nb_thread=%d thread_number=%d blockSize=%d startingPos=%d endPos=%d\n", nb_thread, thread_number, blockSize, startingPos, blockSize + startingPos);
			prev = blockSize + startingPos;
		}
		if (prev != size)
			printf("ERROR\n");
	}
}

void nicolas_hill2(int size, int nb_thread)
{
	int thread_number;
	for (nb_thread=1; nb_thread<=10; nb_thread++) {
		printf("nb_thread=%d\n", nb_thread);
		int prev = 0;
		for(thread_number = 0; thread_number < nb_thread; thread_number++) {
			int blockSize = (int)(size/nb_thread);
			int startingPos = 0;
			printf("size=%d nb_thread=%d thread_number=%d size%%blockSize=%d\n", size, nb_thread, thread_number, size%blockSize);
			if(thread_number < size%blockSize) {
				blockSize++;
				startingPos = thread_number * blockSize;
			} else {
				startingPos = size%blockSize*(blockSize+1);
				startingPos += blockSize*(thread_number - size%blockSize);
			}
			if (prev != startingPos)
				printf("ERROR ");
			printf("nb_thread=%d thread_number=%d blockSize=%d startingPos=%d endPos=%d\n", nb_thread, thread_number, blockSize, startingPos, blockSize + startingPos);
			prev = blockSize + startingPos;
		}
		if (prev != size)
			printf("ERROR\n");
	}
}

void interval_simples(int size, int nb_thread) {
	int thread_number;
	printf("nb_thread=%d\n", nb_thread);
	int prev = 0;
	for(thread_number = 0; thread_number < nb_thread; thread_number++) {
		int start = thread_number * size / nb_thread;
		int end = (thread_number + 1) * size / nb_thread;
		if (prev != start)
			printf("ERROR ");
		printf("start=%d end=%d\n", start, end);
		prev = end;
	}
	if (prev != size)
		printf("ERROR\n");
}

int main(int argc, char **argv)
{
	/* compute_position */
	//test_compute_position_orientation();
	//test_compute_limits();
	//test_rotate();
	//test_draw_dragon();
	//test_compute_limits_set();
	//test_check_limits();
	//test_random_color();
	//test_init_palette();
	int max_size = 1000;
	int max_thread = 10;
	int size, nb_thread;
	for (size=max_thread; size<max_size; size++) {
		for (nb_thread=1; nb_thread<max_thread; nb_thread++) {
			//nicolas_hill(size, nb_thread);
			//nicolas_hill2(size, nb_thread);
			//interval_simples(31, 4);
		}
	}
	interval_simples(5, 10);
	return 0;
}
