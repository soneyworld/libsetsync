/*
 * SQLiteTest.h
 *
 *      Author: Till Lorentzen
 */

#ifndef SQLITETEST_H_
#define SQLITETEST_H_
#include "Test.h"
class SQLiteTest : public Test{
private:
	void testInMemory();
	void testOnFS();
	void testTransactions();
public:
	SQLiteTest();
	virtual void run();
	virtual ~SQLiteTest();
};

#endif /* SQLITETEST_H_ */
