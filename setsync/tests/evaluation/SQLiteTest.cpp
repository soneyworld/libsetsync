/*
 * SQLiteTest.cpp
 *
 *      Author: Till Lorentzen
 */

#include "SQLiteTest.h"
#include <setsync/utils/CryptoHash.h>
#include <sstream>
using namespace std;

SQLiteTest::SQLiteTest() {
	// TODO Auto-generated constructor stub

}

SQLiteTest::~SQLiteTest() {
	// TODO Auto-generated destructor stub
}

void SQLiteTest::testInMemory() {
	sqlite::SQLiteDatabase * db = new sqlite::SQLiteDatabase();
	std::string s = "dfg";
	sqlite::SQLiteIndex index(db, s);
	unsigned int counter = 0;
	unsigned int icounter = 0;
	time_t seconds;
	seconds = time(NULL);
	time_t duration = 0;
	time_t iduration = 0;
	utils::CryptoHash chash;
	bloom::DoubleHashingScheme function(chash.getHashSize());
	for (int i = 0; i < LOOP_ITERATIONS; i++) {
		SHA1Generator generator(i * ITEMS_PER_LOOPS,
				i * ITEMS_PER_LOOPS + ITEMS_PER_LOOPS);
		generator.run();
		for (uint64_t k = 0; k < ITEMS_PER_LOOPS; k++) {
			for (int j = 0; j < function.count(); j++) {
				uint64_t hash = function(generator.array + (k * 20), 20, j);
				index.insert(hash, generator.array + (k * 20), 20);
				counter++;
				icounter++;
			}
			if (seconds + 10 < time(NULL)) {
				index.commit();
				duration += time(NULL) - seconds;
				iduration = time(NULL) - seconds;
				icounter = 0;
				seconds = time(NULL);
				printf(
						"%d inserts after %ld sec (%ld per sec) (%ld per sec in last interval)\n",
						counter, duration, counter / duration,
						icounter / iduration);
			}
		}
	}
	index.commit();
	printf("%d inserts after %ld sec (%ld per sec)\n", counter, duration,
			counter / duration);
}

void SQLiteTest::testOnFS() {
	std::string s = "dfg";
	std::string dbname = "generated.db";
	sqlite::SQLiteDatabase * db = new sqlite::SQLiteDatabase(dbname);
	sqlite::SQLiteIndex index(db, s);
	unsigned int counter = 0;
	unsigned int icounter = 0;
	time_t seconds;
	seconds = time(NULL);
	time_t duration = 0;
	time_t iduration = 0;
	utils::CryptoHash chash;
	bloom::DoubleHashingScheme function(chash.getHashSize());
	for (int i = 0; i < LOOP_ITERATIONS; i++) {
		SHA1Generator generator(i * ITEMS_PER_LOOPS,
				i * ITEMS_PER_LOOPS + ITEMS_PER_LOOPS);
		generator.run();
		for (uint64_t k = 0; k < ITEMS_PER_LOOPS; k++) {
			for (int j = 0; j < function.count(); j++) {
				uint64_t hash = function(generator.array + (k * 20), 20, j);
				index.insert(hash, generator.array + (k * 20), 20);
				counter++;
				icounter++;
			}
			if (seconds + 10 < time(NULL)) {
				index.commit();
				duration += time(NULL) - seconds;
				iduration = time(NULL) - seconds;
				icounter = 0;
				seconds = time(NULL);
				printf(
						"%d inserts after %ld sec (%ld per sec) (%ld per sec in last interval)\n",
						counter, duration, counter / duration,
						icounter / iduration);
			}
		}
	}
	index.commit();
	printf("%d inserts after %ld sec (%ld per sec)\n", counter, duration,
			counter / duration);
}

void SQLiteTest::testTransactions() {
	cout << "running SQLite DB transaction test:" << endl;
	cout << "notrans,interval,duration" << endl;
	remove("generated.db");
	std::string dbname = "generated.db";
	sqlite::SQLiteDatabase * db = new sqlite::SQLiteDatabase(dbname);
	sqlite3 * db_ = db->getConnection();
	int rc =
			sqlite3_exec(db_,
					"CREATE TABLE IF NOT EXISTS test (hash INTEGER);", NULL,
					NULL, NULL);
	int count = 0;
	clock_t start, stop, duration, iduration;
	duration = iduration = 0;
	for (uint64_t i = 0; i < ITERATIONS; i++) {
		stringstream ss;
		ss << "INSERT INTO test VALUES (" << i << ");";
		start = clock();
		sqlite3_exec(db_, ss.str().c_str(), 0, 0, 0);
		stop = clock();
		duration += stop - start;
		iduration += stop - start;
		if (count == ITEMS_PER_LOOPS) {
			cout << i << "," << iduration << "," << duration << endl;
			count = 0;
			iduration = 0;
		}
		count++;
	}
	cout << ITERATIONS << "," << iduration <<"," << duration << endl;
	remove("generated.db");
}

void SQLiteTest::run() {
	testTransactions();
	testInMemory();
	testOnFS();

}
