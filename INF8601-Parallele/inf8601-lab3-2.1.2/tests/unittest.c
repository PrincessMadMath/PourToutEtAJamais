/*
 * unittest.c
 *
 *  Created on: 2011-11-16
 *      Author: francis
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define THREASHOLD 0.000001f

void assert_range(double exp, double act, char *str)
{
    double diff = fabs(exp - act);
    if (diff > THREASHOLD) {
        printf("FAIL: %s diff exp=%f act=%f is over %f\n", str, exp, act, THREASHOLD);
        abort();
    } else {
        printf("PASS: %s\n", str);
    }
}

void assert_equals(int exp, int act, char *str)
{
    if (exp != act) {
        printf("FAIL: %s exp=%d act=%d\n", str, exp, act);
        abort();
    } else {
        printf("PASS: %s\n", str);
    }
}


