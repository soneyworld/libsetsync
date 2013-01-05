/*
 * SetTest.cpp
 *
 *      Author: Till Lorentzen
 */

#include "SetTest.h"
#ifdef HAVE_LEVELDB
#include <setsync/storage/LevelDbStorage.h>
#endif
#include <setsync/utils/FileSystem.h>
#include "StopWatch.h"

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
	cout
			<< "noinserts,CPUinterval,realtimeInterval,CPUduration,realtimeDuration,foldersize,"
			<< endl;
	StopWatch watch;
	double CPUinterval;
	double RTinterval;
	for (uint64_t i = 0; i < ITERATIONS; i++) {
		watch.start();
		set_.insert((void *) &i, sizeof(uint64_t));
		watch.stop();
		CPUinterval += watch.getLastCPUDuration();
		RTinterval += watch.getLastDuration();
		if (((i + 1) % ITEMS_PER_LOOPS) == 0) {
			std::cout << i + 1 << "," << CPUinterval << "," << RTinterval
					<< "," << watch.getCPUDuration() << ","
					<< watch.getDuration() << ","
					<< utils::FileSystem::dirSize(set_.getPath()) << std::endl;
			CPUinterval = 0;
			RTinterval = 0;
		}
	}
}

void SetTest::run() {
	insert();
}

}
