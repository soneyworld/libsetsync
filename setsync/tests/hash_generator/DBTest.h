/*
 * DBTest.h
 *
 *      Author: Till Lorentzen
 */

#ifndef DBTEST_H_
#define DBTEST_H_
#include "Test.h"
#include <utility>
#include <db_cxx.h>

class DBTest: public Test {
private:
	void runMemDb();
	void runFsDb();
	void put(Db * db, std::pair<Dbt, Dbt> values);
	void get(Db * db, Dbt key);
	void del(Db * db, Dbt key);
public:
	DBTest();
	void run();
	virtual ~DBTest();
};

#endif /* DBTEST_H_ */
