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
	void runDbSizeTest(const DBTYPE type, const size_t keysize, const size_t valuesize);
	void runDbSizeTest();
	void runMemDb();
	void runFsDb();
	void runTrie(Db * db);
	void runMemDbTrie();
	void runFsDbTrie();
	void put(Db * db, std::pair<Dbt, Dbt> values);
	void get(Db * db, Dbt key);
	void del(Db * db, Dbt key);
public:
	DBTest();
	void run();
	virtual ~DBTest();
};

#endif /* DBTEST_H_ */
