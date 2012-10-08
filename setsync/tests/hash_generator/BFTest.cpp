/*
 * BFTest.cpp
 *
 *      Author: Till Lorentzen
 */

#include "BFTest.h"
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
	//	cout << "running Berkeley DBBloomFilter test:"<<endl;
	//	bloom::DBBloomFilter bf(ITERATIONS);
	//	runBF(&bf);
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
