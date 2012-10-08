/*
 * BFTest.h
 *
 *      Author: Till Lorentzen
 */

#ifndef BFTEST_H_
#define BFTEST_H_
#include "Test.h"
#include "SHA1Generator.h"
#include <setsync/bloom/BloomFilter.h>
#include <setsync/bloom/FSBloomFilter.h>
#include <setsync/index/SQLiteDatabase.h>
#include <setsync/index/SQLiteIndex.h>
#include <setsync/bloom/HashFunction.h>
#include <db_cxx.h>


class BFTest : public Test{
public:
	BFTest();
	void run();
	virtual ~BFTest();
};

#endif /* BFTEST_H_ */
