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
	case setsync::config::Configuration::StorageConfig::IN_MEMORY:
		this->storage_ = "MEM_DB";
		break;
	}

}

SetTest::~SetTest() {

}

void SetTest::insert() {

	cout << "running Set insertion test (" << storage_ << ")" << endl;
	cout << "noinserts,realtimeInterval,realtimeDuration" << endl;
	StopWatch watch;
	double RTinterval;
	for (uint64_t i = 0; i < set_.getMaximumSize(); i++) {
		watch.start();
		set_.insert((void *) &i, sizeof(uint64_t));
		watch.stop();
		RTinterval += watch.getLastDuration();
		if (((i + 1) % ITEMS_PER_LOOPS) == 0) {
			std::cout << i + 1 << "," << RTinterval << ","
					<< watch.getDuration() << std::endl;
			RTinterval = 0;
		}
	}
	cout << "########################" << endl;
	cout << "DirectorySize=" << setsync::utils::FileSystem::dirSize(set_.getPath()) << endl;
}

void SetTest::run() {
	insert();
}

}
