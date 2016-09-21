/*
 * TidMap.h
 *
 *  Created on: 2011-08-27
 *      Author: francis
 */

#ifndef TIDMAP_H_
#define TIDMAP_H_

#include <iostream>
#include "tbb/tbb.h"

using std::ostream;
using namespace tbb;
using namespace std;

typedef spin_rw_mutex ArrayMutex;

class TidMap {
private:
	int size;
	int pos;
	int *array;
	ArrayMutex mutex;
public:
	TidMap(int size);
	virtual ~TidMap();
	int getIdFromTid(const int tid);
	void dump();
};

#endif /* TIDMAP_H_ */
