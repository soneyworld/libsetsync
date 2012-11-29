/*
 * DBTest.cpp
 *
 *      Author: Till Lorentzen
 */

#include "DBTest.h"
#include <setsync/trie/BdbTrie.h>
#include <sstream>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
using namespace std;
DBTest::DBTest() {

}

DBTest::~DBTest() {
}

void DBTest::run() {
	runDbSizeTestInSteps();
	runDbSizeTest();
	runMemDb();
	runFsDb();
	runMemDbTrie();
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
	utils::CryptoHash sha1;
	trie::BdbTrie trie(sha1, db);
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
		double itemsPerSecond = ITEMS_PER_LOOPS / ((double) (iduration)
				/ CLOCKS_PER_SEC);
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
	db.set_cachesize(5, 0, 0);
	db.open(NULL, NULL, trie::BdbTrie::getLogicalDatabaseName(),
			trie::BdbTrie::getTableType(), DB_CREATE, 0);
	runTrie(&db);
	db.stat_print(0);
	db.close(0);
}

void DBTest::runFsDbTrie() {
	cout << "running Berkeley DB Trie(fs) test:" << endl;
	Db db(NULL, 0);
	db.set_cachesize(5, 0, 0);
	db.open(NULL, "temp-table.db", trie::BdbTrie::getLogicalDatabaseName(),
			trie::BdbTrie::getTableType(), DB_CREATE, 0);
	runTrie(&db);
	db.sync(0);
	db.stat_print(0);
	db.close(0);
	remove("temp-table.db");
}

void DBTest::runDbSizeTest(const DBTYPE type, const size_t keysize,
		const size_t valuesize) {
	size_t keysize_;
	if (keysize < sizeof(uint64_t))
		keysize_ = sizeof(uint64_t);
	else
		keysize_ = keysize;
	string t;
	switch (type) {
	case DB_HASH:
		t = "DB_HASH";
		break;
	case DB_BTREE:
		t = "DB_BTREE";
		break;
	default:
		t = "unknown type";
		break;
	}
	cout << "running Berkeley DB Size test (" << t << "): " << endl;
	remove("temp-table.db");
	Db db(NULL, 0);
	db.set_cachesize(5, 0, 0);
	db.open(NULL, "temp-table.db", NULL, type, DB_CREATE, 0);
	cout << "noentries,keysize,valuesize,filesize,sizePerEntry,sizePerByte"
			<< endl;
	unsigned char keybuf[keysize_];
	unsigned char valuebuf[valuesize];
	for (uint64_t i = 0; i < ITERATIONS; i++) {
		cout << i+1 << ",";
		memcpy(keybuf, &i, sizeof(uint64_t));
		Dbt key(keybuf, sizeof(uint64_t));
		cout << key.get_size() << ",";
		Dbt value(valuebuf, valuesize);
		cout << value.get_size() << ",";
		pair<Dbt, Dbt> p(key, value);
		put(&db, p);
		db.sync(0);
		struct stat filestatus;
		stat("temp-table.db", &filestatus);
		double sizePerEntry = ((double)filestatus.st_size) / (double)(i + 1);
		double sizePerByte = ((double) filestatus.st_size) / (double) ((i + 1) * (keysize_
				+ valuesize));
		cout << filestatus.st_size << "," << sizePerEntry << "," << sizePerByte
				<< endl;
	}
	db.close(0);
	remove("temp-table.db");

}

void DBTest::runDbSizeTestInSteps(const DBTYPE type, const size_t keysize,
		const size_t valuesize) {
	size_t keysize_;
	if (keysize < sizeof(uint64_t))
		keysize_ = sizeof(uint64_t);
	else
		keysize_ = keysize;
	string t;
	switch (type) {
	case DB_HASH:
		t = "DB_HASH";
		break;
	case DB_BTREE:
		t = "DB_BTREE";
		break;
	default:
		t = "unknown type";
		break;
	}
	cout << "running Berkeley DB Size test (" << t << "): " << endl;
	remove("temp-table.db");
	Db db(NULL, 0);
	db.set_cachesize(5, 0, 0);
	db.open(NULL, "temp-table.db", NULL, type, DB_CREATE, 0);
	cout << "noentries,keysize,valuesize,filesize,sizePerEntry,sizePerByte"
			<< endl;
	unsigned char keybuf[keysize_];
	unsigned char valuebuf[valuesize];
	uint64_t i = 0;
	for (uint64_t iter = 0; iter < LOOP_ITERATIONS; iter++) {
		for(unsigned int j = 0; j < ITEMS_PER_LOOPS; j++){
			memcpy(keybuf, &i, sizeof(uint64_t));
			Dbt key(keybuf, sizeof(uint64_t));
			Dbt value(valuebuf, valuesize);
			pair<Dbt, Dbt> p(key, value);
			put(&db, p);
			i++;
		}
		cout << i << "," << keysize_ <<","<< valuesize << ",";
		db.sync(0);
		struct stat filestatus;
		stat("temp-table.db", &filestatus);
		double sizePerEntry = ((double)filestatus.st_size) / (double)(i );
		double sizePerByte = ((double) filestatus.st_size) / (double) ((i ) * (keysize_
				+ valuesize));
		cout << filestatus.st_size << "," << sizePerEntry << "," << sizePerByte
				<< endl;
	}
	db.close(0);
	remove("temp-table.db");

}

void DBTest::runDbSizeTest() {
	runDbSizeTest(DB_BTREE, 8, 0);
	runDbSizeTest(DB_HASH, 8, 0);
	runDbSizeTest(DB_BTREE, 16, 0);
	runDbSizeTest(DB_HASH, 16, 0);
	runDbSizeTest(DB_BTREE, 20, 0);
	runDbSizeTest(DB_HASH, 20, 0);
	runDbSizeTest(DB_BTREE, 20, 100);
	runDbSizeTest(DB_HASH, 20, 100);
	runDbSizeTest(DB_BTREE, 20, 200);
	runDbSizeTest(DB_HASH, 20, 200);
}

void DBTest::runDbSizeTestInSteps() {
	runDbSizeTestInSteps(DB_BTREE, 8, 0);
	runDbSizeTestInSteps(DB_HASH, 8, 0);
	runDbSizeTestInSteps(DB_BTREE, 16, 0);
	runDbSizeTestInSteps(DB_HASH, 16, 0);
	runDbSizeTestInSteps(DB_BTREE, 20, 0);
	runDbSizeTestInSteps(DB_HASH, 20, 0);
	runDbSizeTestInSteps(DB_BTREE, 20, 100);
	runDbSizeTestInSteps(DB_HASH, 20, 100);
	runDbSizeTestInSteps(DB_BTREE, 20, 200);
	runDbSizeTestInSteps(DB_HASH, 20, 200);
}
