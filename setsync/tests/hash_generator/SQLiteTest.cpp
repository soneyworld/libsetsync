/*
 * SQLiteTest.cpp
 *
 *      Author: Till Lorentzen
 */

#include "SQLiteTest.h"

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
	bloom::DoubleHashingScheme function;
	for (int i = 0; i < LOOP_ITERATIONS; i++) {
		SHA1Generator generator(i * ITEMS_PER_LOOPS,
				i * ITEMS_PER_LOOPS + ITEMS_PER_LOOPS);
		generator.run();
		for (uint64_t k = 0; k < ITEMS_PER_LOOPS; k++) {
			for (int j = 0; j < function.count(); j++) {
				uint64_t hash =
						function.hash(generator.array + (k * 20), 20, j);
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
	bloom::DoubleHashingScheme function;
	for (int i = 0; i < LOOP_ITERATIONS; i++) {
		SHA1Generator generator(i * ITEMS_PER_LOOPS,
				i * ITEMS_PER_LOOPS + ITEMS_PER_LOOPS);
		generator.run();
		for (uint64_t k = 0; k < ITEMS_PER_LOOPS; k++) {
			for (int j = 0; j < function.count(); j++) {
				uint64_t hash =
						function.hash(generator.array + (k * 20), 20, j);
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

void SQLiteTest::run() {
	testInMemory();
	testOnFS();

}
