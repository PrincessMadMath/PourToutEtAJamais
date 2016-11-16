/*
 * test_grid.c
 *
 *  Created on: 2011-11-15
 *      Author: francis
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "grid.h"
#include "unittest.h"

void test_grid_simple()
{
	int width = 4;
	int height = 4;
	int padding = 1;
	double exp, act;
	double v0 = 0.0;
	double v1 = 1.0;
	grid_t *g1, *g2;

	/* all elements are set */
	g1 = make_grid(width, height, padding);
	int cnt = ((width + padding * 2) * (height + padding * 2)) - 1;
	exp = cnt * ceil((double)cnt / 2);
	grid_set_increment(g1);
	grid_sum(g1, &act);
	assert_range(exp, act, "test_grid_set");

	/* only inner is set */
	exp = width * height * v1;
	grid_set(g1, v0);
	grid_set_inner(g1, v1);
	grid_sum(g1, &act);
	assert_range(exp, act, "test_grid_set_inner");

	free_grid(g1);

	/* copy into new grid without padding */
	g1 = make_grid(4, 4, 1);
	g2 = make_grid(4, 4, 0);
	grid_set(g1, v0);
	grid_set_increment(g2);
	grid_copy(g2, g1);
	grid_sum(g2, &exp);
	grid_sum(g1, &act);
	assert_range(exp, act, "test_grid_copy1");

	grid_set_increment(g1);
	grid_set(g2, v0);
	grid_copy(g1, g2);
	grid_sum(g2, &exp);
	grid_sum(g1, &act);
	assert_range(7.0, g2->dbl[0], "test_grid_copy2_1");
	assert_range(28.0, g2->dbl[15], "test_grid_copy2_2");
	free_grid(g1);
	free_grid(g2);

	g1 = make_grid(10, 10, 0);
	g2 = make_grid(4, 4, 0);
	grid_set_increment(g1);
	grid_set(g2, v0);
	grid_copy_block(g1, 2, 6, 4, 4, g2, 0, 0);
	grid_sum(g2, &act);
	assert_range(62.0, g2->dbl[0], "test_grid_copy_block_1");
	assert_range(95.0, g2->dbl[15], "test_grid_copy_block_2");
	free_grid(g1);
	free_grid(g2);

	g1 = make_grid(4, 4, 0);
	g2 = make_grid(10, 10, 0);
	grid_set_increment(g1);
	grid_set(g2, v0);
	grid_copy_block(g1, 0, 0, 4, 4, g2, 2, 6);
	grid_sum(g1, &exp);
	grid_sum(g2, &act);
/*
	printf("g1\n");
	dump_grid(g1);
	printf("g2\n");
	dump_grid(g2);
*/
	assert_range(exp, act, "test_grid_copy_block_3");
	free_grid(g1);
	free_grid(g2);

}

void test_grid_padding()
{
    double exp, act;

    grid_t *g1 = make_grid(10, 10, 0);
    grid_set(g1, 1.0);
    grid_t *g2 = grid_padding(g1, 1);
    grid_sum(g1, &exp);
    grid_sum(g2, &act);
    assert_equals(10, g2->width, "test_grid_padding_1");
    assert_equals(10, g2->height, "test_grid_padding_2");
    assert_equals(12, g2->pw, "test_grid_padding_3");
    assert_equals(12, g2->ph, "test_grid_padding_4");
    assert_range(exp, act, "test_grid_padding_sum_1");

    grid_t *g3 = grid_padding(g2, 0);
    grid_sum(g3, &act);
    assert_equals(10, g3->width, "test_grid_padding_5");
    assert_equals(10, g3->height, "test_grid_padding_6");
    assert_equals(10, g3->pw, "test_grid_padding_7");
    assert_equals(10, g3->ph, "test_grid_padding_8");
    assert_range(exp, act, "test_grid_padding_sum_2");
    free_grid(g1);
    free_grid(g2);
    free_grid(g3);
}

void test_grid_set_min()
{
	double exp, act;
	grid_t *g1 = make_grid(10, 10, 0);
	grid_t *g2 = make_grid(10, 10, 0);
	grid_set_increment(g1);
	grid_set(g2, 50.0);
	grid_set_min(g2, g1);

	/*
	printf("g1\n");
	dump_grid(g1);
	printf("g2\n");
	dump_grid(g2);
	*/
	exp = 6225.0;
	grid_sum(g1, &act);
	assert_range(exp, act, "test_grid_set_min");
	free_grid(g1);
	free_grid(g2);
}

void test_grid_set_bounds()
{
	double exp, act;
	grid_t *g1 = make_grid(10, 10, 0);
	grid_set_increment(g1);

	/*
	printf("before\n");
	dump_grid(g1);
	*/
	grid_set_bounds1(g1);
	/*
	printf("after\n");
	dump_grid(g1);
	*/
	exp = 4950.0;
	grid_sum(g1, &act);
	assert_range(exp, act, "test_grid_set_bounds1");
	free_grid(g1);
}

int main(int argc, char **argv)
{
	test_grid_simple();
	test_grid_padding();
	test_grid_set_min();
	test_grid_set_bounds();
	return 0;
}
