/*
 * Test.h
 *
 *      Author: Till Lorentzen
 */

#ifndef TEST_H_
#define TEST_H_
#define GIGABYTE 1024*1024*1024
#define ITERATIONS 1000000
#define ITEMS_PER_LOOPS 1000
#define LOOP_ITERATIONS ITERATIONS/ITEMS_PER_LOOPS
#include "config.h"
#include <time.h>
#include <stdio.h>
#include <iostream>
#include <omp.h>
#include <stdint.h>
#include "SHA1Generator.h"
#include <setsync/bloom/DoubleHashingScheme.h>

class Test{
public:
	virtual void run() = 0;
};

#endif /* TEST_H_ */
