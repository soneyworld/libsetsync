/*
 * SQLiteTest.cpp
 *
 *      Author: Till Lorentzen
 */

#include "SQLiteTest.h"
#include <setsync/crypto/CryptoHash.h>
#include <sstream>
#include <time.h>
#include "StopWatch.h"
#include "SQLiteDatabase.h"

using namespace std;

SQLiteTest::SQLiteTest() {
	// TODO Auto-generated constructor stub

}

SQLiteTest::~SQLiteTest() {
	// TODO Auto-generated destructor stub
}


void SQLiteTest::testTransactions() {
	cout << "running SQLite DB transaction test:" << endl;
	cout << "notrans,CPUintervalDuration,realtimeIntervalDuration,duration"
			<< endl;
	remove("generated.db");
	std::string dbname = "generated.db";
	sqlite::SQLiteDatabase * db = new sqlite::SQLiteDatabase(dbname);
	sqlite3 * db_ = db->getConnection();
	int rc =
			sqlite3_exec(db_,
					"CREATE TABLE IF NOT EXISTS test (hash INTEGER);", NULL,
					NULL, NULL);
	int count = 0;
	double duration = 0;
	StopWatch stopwatch;
	for (uint64_t i = 0; i < ITERATIONS; i++) {
		stringstream ss;
		ss << "INSERT INTO test VALUES (" << i << ");";
		stopwatch.start();
		sqlite3_exec(db_, ss.str().c_str(), 0, 0, 0);
		stopwatch.stop();
		if (count == ITEMS_PER_LOOPS) {
			duration += stopwatch.getDuration();
			cout << i << "," << stopwatch.getCPUDuration() << ","
					<< stopwatch.getDuration() << "," << duration << endl;
			count = 0;
			stopwatch.reset();
		}
		count++;
	}
	duration += stopwatch.getDuration();
	cout << ITERATIONS << "," << stopwatch.getCPUDuration() << ","
			<< stopwatch.getDuration() << "," << duration << endl;
	remove("generated.db");
}

void SQLiteTest::run() {
	testTransactions();
	//testInMemory();
	//testOnFS();

}
