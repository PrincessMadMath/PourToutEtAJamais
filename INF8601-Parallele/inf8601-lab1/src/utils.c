/*
 * utils.c
 *
 *  Created on: 2011-08-27
 *      Author: francis
 */

#include <sys/syscall.h>
#include <unistd.h>

/* our own implementation of gettid specific to Linux */
int gettid()
{
	return (int) syscall(SYS_gettid);
}
