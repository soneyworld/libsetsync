/*
 * SpeedTest.cpp
 *
 *      Author: Till Lorentzen
 */

#include "SpeedTest.h"
#include <sstream>
#include "StopWatch.h"

namespace evaluation {
using namespace std;

SpeedTest::SpeedTest(AbstractKeyValueStorage& storage) :
	storage_(storage) {

}

SpeedTest::~SpeedTest() {
}

void SpeedTest::run() {
	cout << "operation,number,duration,phaseduration" << endl;
	unsigned char buffer[hash.getHashSize()];
	unsigned char value[200];
	StopWatch watch;
	StopWatch duration;
	for (int iter = 0; iter < LOOP_ITERATIONS; iter++) {
		for (int i = 0; i < ITEMS_PER_LOOPS; i++) {
			stringstream ss;
			ss << "test" << iter * ITEMS_PER_LOOPS + i;
			hash(buffer, ss.str());
			duration.start();
			watch.start();
			storage_.put(buffer, hash.getHashSize(), value, 200);
			watch.stop();
			duration.stop();
		}
		cout << "insert," << iter * ITEMS_PER_LOOPS + ITEMS_PER_LOOPS << ","
				<< duration.getDuration() << "," << watch.getDuration() << endl;
		watch.reset();
	}
	duration.reset();
	for (int iter = 0; iter < LOOP_ITERATIONS; iter++) {
		unsigned char * valuepnt;
		size_t valuesize;
		for (int i = 0; i < ITEMS_PER_LOOPS; i++) {
			stringstream ss;
			ss << "test" << iter * ITEMS_PER_LOOPS + i;
			hash(buffer, ss.str());
			duration.start();
			watch.start();
			storage_.get(buffer, hash.getHashSize(), &valuepnt, &valuesize);
			watch.stop();
			duration.stop();
			delete valuepnt;
		}
		cout << "get," << iter * ITEMS_PER_LOOPS + ITEMS_PER_LOOPS << ","
				<< watch.getDuration() << "," << duration.getDuration() << endl;
		watch.reset();
	}
	duration.reset();
	for (int iter = 0; iter < LOOP_ITERATIONS; iter++) {
		for (int i = 0; i < ITEMS_PER_LOOPS; i++) {
			stringstream ss;
			ss << "test" << iter * ITEMS_PER_LOOPS + i;
			hash(buffer, ss.str());
			duration.start();
			watch.start();
			storage_.del(buffer, hash.getHashSize());
			watch.stop();
			duration.stop();
		}
		cout << "del," << iter * ITEMS_PER_LOOPS + ITEMS_PER_LOOPS << ","
				<< watch.getDuration() << "," << duration.getDuration() << endl;
		watch.reset();
	}
}
}
