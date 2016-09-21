/*
 * TidMap.cpp
 *
 *  Created on: 2011-08-27
 *      Author: francis
 */

#include <iostream>

#include "tbb/tbb.h"
#include "TidMap.h"

using std::ostream;
using namespace tbb;
using namespace std;

TidMap::TidMap(int size) : size(size), pos(0) {
	array = new int[size];
	int i;
	for (i = 0; i < size; i++) {
		array[i] = 0;
	}
}

TidMap::~TidMap() {
	delete[] array;
}

int TidMap::getIdFromTid(const int tid) {
	/* start with a read only lock */
	ArrayMutex::scoped_lock lock(mutex, false);
	int i;
	for (i = 0; i < pos; i++) {
		if (tid == array[i])
			return i;
	}
	/* the value is not found, add it to the array
	 * updrade to write lock before modification */
	if (!lock.upgrade_to_writer()) {
		/* upgrade_to_write may release the lock for a short period
		 * of time, check again if another thread already added the value */
		for (i = 0; i < pos; i++) {
			if (tid == array[i])
				return i;
		}
	}
	/*
	 * write lock is held here and
	 * the value is not in the array
	 */

	/* detect full condition */
	if (pos == size)
		return -1;

	/* append to the array, increase position */
	int ret = pos;
	array[pos++] = tid;
	return ret;
	/* lock is released automatically */
}

void TidMap::dump() {
	ArrayMutex::scoped_lock lock(mutex, false);
	int i;
	cout << "{ ";
	for (i = 0; i < size; i++) {
		cout << i << "=" << array[i] << " ";
	}
	cout << "}\n";
}
