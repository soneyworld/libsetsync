/*
 * DBTest.h
 *
 *      Author: Till Lorentzen
 */

#ifndef DBTEST_H_
#define DBTEST_H_
#include "Test.h"

class DBTest :public Test{
public:
	DBTest();
	void run();
	virtual ~DBTest();
};

#endif /* DBTEST_H_ */
