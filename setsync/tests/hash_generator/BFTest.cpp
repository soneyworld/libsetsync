/*
 * BFTest.cpp
 *
 *      Author: Till Lorentzen
 */

#include "BFTest.h"

BFTest::BFTest() {
	// TODO Auto-generated constructor stub

}

BFTest::~BFTest() {
	// TODO Auto-generated destructor stub
}

void BFTest::run() {
	uint64_t stepsize = ITEMS_PER_LOOPS;
	bloom::BloomFilter all(ITERATIONS);
	unsigned int counter = 0;
	unsigned int icounter = 0;
	//	int thread_id;
	time_t act_time;
	time_t duration = 0;
	time_t iduration = 0;
	//#pragma omp parallel for schedule(static) private(thread_id)
	for (int i = 0; i < LOOP_ITERATIONS; i++) {
		SHA1Generator generator(i * ITEMS_PER_LOOPS,
				i * ITEMS_PER_LOOPS + ITEMS_PER_LOOPS);
		generator.run();
		act_time = time(NULL);
		all.addAll(generator.array, ITEMS_PER_LOOPS);
		iduration = time(NULL) - act_time;
		duration += iduration;
		counter += ITEMS_PER_LOOPS;
	}
	printf("%d inserts after %ld sec (%ld per sec)\n",
			counter, duration, counter / duration);

}
