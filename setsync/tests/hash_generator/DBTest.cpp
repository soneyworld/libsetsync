/*
 * DBTest.cpp
 *
 *      Author: Till Lorentzen
 */

#include "DBTest.h"
#include <setsync/trie/DBTrie.h>
#include <sstream>
using namespace std;
DBTest::DBTest() {

}

DBTest::~DBTest() {
}

void DBTest::run() {
	//	runMemDb();
	runFsDb();
	//	runMemDbTrie();
	runFsDbTrie();
}

void DBTest::put(Db * db, pair<Dbt, Dbt> values) {
	db->put(NULL, &(values.first), &(values.second), 0);
}
void DBTest::get(Db * db, Dbt key) {
	Dbt value;
	db->get(NULL, &key, &value, 0);

}
void DBTest::del(Db * db, Dbt key) {
	db->del(NULL, &key, 0);
}

void DBTest::runMemDb() {
	cout << "running Berkeley DB (mem) test:" << endl;
	Db db(NULL, 0);
	db.open(NULL, NULL, NULL, DB_BTREE, DB_CREATE, 0);
	clock_t start, stop, duration;
	duration = 0;
	for (int iter = 0; iter < LOOP_ITERATIONS; iter++) {
		start = clock();
		for (int i = 0; i < ITEMS_PER_LOOPS; i++) {
			Dbt key(&i, sizeof(int));
			Dbt value(NULL, 0);
			pair<Dbt, Dbt> p(key, value);
			put(&db, p);
		}
		for (int i = 0; i < ITEMS_PER_LOOPS; i++) {
			Dbt key(&i, sizeof(int));
			get(&db, key);
		}
		for (int i = 0; i < ITEMS_PER_LOOPS; i++) {
			Dbt key(&i, sizeof(int));
			del(&db, key);
		}
		stop = clock();
		duration += stop - start;
	}
	cout << ITERATIONS << " adds/gets/inserts after " << (float) duration
			/ CLOCKS_PER_SEC;
	cout << " sec (~" << (long) ((long) ITERATIONS / (float) (duration
			/ CLOCKS_PER_SEC)) << " per sec)" << endl;
	db.close(0);
}

void DBTest::runFsDb() {
	cout << "running Berkeley DB (fs) test:" << endl;
	Db db(NULL, 0);
	db.open(NULL, "temp-table.db", "test", DB_BTREE, DB_CREATE, 0);
	clock_t start, stop, duration;
	duration = 0;
	for (int iter = 0; iter < LOOP_ITERATIONS; iter++) {
		start = clock();
		for (int i = 0; i < ITEMS_PER_LOOPS; i++) {
			Dbt key(&i, sizeof(int));
			Dbt value(NULL, 0);
			pair<Dbt, Dbt> p(key, value);
			put(&db, p);
		}
		db.sync(0);
		for (int i = 0; i < ITEMS_PER_LOOPS; i++) {
			Dbt key(&i, sizeof(int));
			get(&db, key);
		}
		for (int i = 0; i < ITEMS_PER_LOOPS; i++) {
			Dbt key(&i, sizeof(int));
			del(&db, key);
		}
		stop = clock();
		duration += stop - start;
	}
	cout << ITERATIONS << " adds/gets/inserts after " << (float) duration
			/ CLOCKS_PER_SEC;
	cout << " sec (~" << (long) ((long) ITERATIONS / (float) (duration
			/ CLOCKS_PER_SEC)) << " per sec)" << endl;
	db.close(0);
	remove("temp-table.db");
}

void DBTest::runTrie(Db * db) {
	trie::DBTrie trie(db);
	clock_t start, stop, duration, iduration;
	duration = 0;
	for (int iter = 0; iter < LOOP_ITERATIONS; iter++) {
		iduration = 0;
		start = clock();
		for (int i = 0; i < ITEMS_PER_LOOPS; i++) {
			stringstream ss;
			ss << iter * ITEMS_PER_LOOPS + i;
			trie.Trie::add(ss.str());
		}
		stop = clock();
		duration += stop - start;
		iduration = stop - start;
		double itemsPerSecond = ITEMS_PER_LOOPS/((double)(iduration) / CLOCKS_PER_SEC);
		cout << ITEMS_PER_LOOPS * iter << ": " << ITEMS_PER_LOOPS
				<< " inserts after " << (float) (iduration) / CLOCKS_PER_SEC;
		cout << " sec (~" << (long) itemsPerSecond << " per sec)" << endl;
	}
	cout << ITERATIONS << " inserts after " << (float) duration
			/ CLOCKS_PER_SEC;
	cout << " sec (~" << (long) ((long) ITERATIONS / (float) (duration
			/ CLOCKS_PER_SEC)) << " per sec)" << endl;
}

void DBTest::runMemDbTrie() {
	cout << "running Berkeley DB Trie(mem) test:" << endl;
	Db db(NULL, 0);
	db.open(NULL, NULL, trie::DBTrie::getLogicalDatabaseName(),
			trie::DBTrie::getTableType(), DB_CREATE, 0);
	runTrie(&db);
	db.close(0);
}

void DBTest::runFsDbTrie() {
	cout << "running Berkeley DB Trie(fs) test:" << endl;
	Db db(NULL, 0);
	db.open(NULL, "temp-table.db", trie::DBTrie::getLogicalDatabaseName(),
			trie::DBTrie::getTableType(), DB_CREATE, 0);
	runTrie(&db);
	db.close(0);
	remove("temp-table.db");
}
