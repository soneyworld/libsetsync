/*
 * BDBInsertRemoveTest.h
 *
 *      Author: Till Lorentzen
 */

#ifndef BDBINSERTREMOVETEST_H_
#define BDBINSERTREMOVETEST_H_
#include "Test.h"
#include <db_cxx.h>

class BDBInsertRemoveTest : public Test {
private:
	Db * db;
	void fillDB();
	void insertRemove();
	const DBTYPE type_;
	unsigned int getNumberOfEntries();
public:
	BDBInsertRemoveTest(const DBTYPE type);
	virtual ~BDBInsertRemoveTest();
	void run();
};

#endif /* BDBINSERTREMOVETEST_H_ */
