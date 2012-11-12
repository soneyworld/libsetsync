/*
 * BDBTransactionsTest.h
 *
 *      Author: Till Lorentzen
 */

#ifndef BDBTRANSACTIONSTEST_H_
#define BDBTRANSACTIONSTEST_H_

#include "Test.h"
#include <db_cxx.h>

class BDBTransactionsTest: public Test {
private:
	DbEnv * env_;
	Db * db_;
public:
	BDBTransactionsTest(const DBTYPE type);
	virtual ~BDBTransactionsTest();
	void run();
};

#endif /* BDBTRANSACTIONSTEST_H_ */
