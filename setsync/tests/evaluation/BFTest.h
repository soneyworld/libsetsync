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
#include <db_cxx.h>


class BFTest : public Test{
private:
	void runBF(bloom::AbstractBloomFilter * bf);
	void runMemBF();
	void runFSBF();
	void runDBBF();
public:
	BFTest();
	void run();
	virtual ~BFTest();
};

#endif /* BFTEST_H_ */
