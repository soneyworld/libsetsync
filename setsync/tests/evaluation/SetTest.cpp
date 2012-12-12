/*
 * SetTest.cpp
 *
 *      Author: Till Lorentzen
 */

#include "SetTest.h"
#ifdef HAVE_LEVELDB
#include <setsync/storage/LevelDbStorage.h>
#endif
using namespace std;
namespace evaluation {

SetTest::SetTest(const setsync::config::Configuration& c) :
	set_(c) {
	switch (c.getStorage().getType()) {
	case setsync::config::Configuration::StorageConfig::LEVELDB:
		this->storage_ = "LEVELDB";
		break;
	case setsync::config::Configuration::StorageConfig::BERKELEY_DB:
		this->storage_ = "BERKELEY_DB";
		break;
	}

}

SetTest::~SetTest() {

}

void SetTest::insert() {

	cout << "running Set insertion test (" << storage_ << ")" << endl;
	cout << "noinserts,CPUintervalDuration,realtimeIntervalDuration,duration"
			<< endl;
	for (uint64_t i = 0; i < ITERATIONS; i++) {
		set_.insert((void *) &i, sizeof(uint64_t));
		if (((i + 1) % ITEMS_PER_LOOPS) == 0) {
			std::cout << i + 1 << "," << std::endl;
		}
	}
}

void SetTest::run() {
	insert();
}

}
