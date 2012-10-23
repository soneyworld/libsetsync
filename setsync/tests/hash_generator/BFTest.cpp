/*
 * BFTest.cpp
 *
 *      Author: Till Lorentzen
 */

#include "BFTest.h"
#include <db_cxx.h>
using namespace std;

BFTest::BFTest() {
	// TODO Auto-generated constructor stub

}

BFTest::~BFTest() {
	// TODO Auto-generated destructor stub
}

void BFTest::run() {
	runMemBF();
	runFSBF();
	runDBBF();
}
void BFTest::runMemBF() {
	cout << "running BloomFilter test:" << endl;
	bloom::BloomFilter bf(ITERATIONS);
	runBF(&bf);
}
void BFTest::runFSBF() {
	cout << "running FSBloomFilter test:" << endl;
	bloom::FSBloomFilter bf(ITERATIONS);
	runBF(&bf);
}
void BFTest::runDBBF() {
	cout << "running Berkeley DBBloomFilter(MEMDB) test:" << endl;
	Db db1(NULL, 0);
	db1.set_flags(bloom::DBBloomFilter::getTableFlags());
	db1.open(NULL, NULL, bloom::DBBloomFilter::getLogicalDatabaseName(),
			bloom::DBBloomFilter::getTableType(), DB_CREATE, 0);
	// Setting Cache to 8 GB
	db1.set_cachesize(8, 0, 0);
	bloom::DBBloomFilter bf(&db1, ITERATIONS);
	runBF(&bf);
	db1.close(0);
	cout << "running Berkeley DBBloomFilter(FS) test:" << endl;
	Db db2(NULL, 0);
	db2.set_flags(bloom::DBBloomFilter::getTableFlags());
	db2.open(NULL, "temp-table.db",
			bloom::DBBloomFilter::getLogicalDatabaseName(),
			bloom::DBBloomFilter::getTableType(), DB_CREATE, 0);

	bloom::DBBloomFilter bf2(&db2, ITERATIONS);
	runBF(&bf2);
	db2.close(0);
	remove("temp-table.db");
}

void BFTest::runBF(bloom::AbstractBloomFilter * bf) {
	uint64_t stepsize = ITEMS_PER_LOOPS;

	clock_t start, stop, duration;
	duration = 0;
	for (int i = 0; i < LOOP_ITERATIONS; i++) {
		SHA1Generator generator(i * ITEMS_PER_LOOPS,
				i * ITEMS_PER_LOOPS + ITEMS_PER_LOOPS);
		generator.run();
		start = clock();
		bf->addAll(generator.array, ITEMS_PER_LOOPS);
		stop = clock();
		duration += stop - start;
	}
	cout << ITERATIONS << " inserts after " << (float) duration
			/ CLOCKS_PER_SEC;
	cout << " sec (~" << (long) ((long) ITERATIONS / (float) (duration
			/ CLOCKS_PER_SEC)) << " per sec)" << endl;
}
