/*
 * unittest.h
 *
 *  Created on: 2011-11-16
 *      Author: francis
 */

#ifndef UNITTEST_H_
#define UNITTEST_H_

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

void assert_range(double exp, double act, char *str);
void assert_equals(int exp, int act, char *str);


#endif /* UNITTEST_H_ */
