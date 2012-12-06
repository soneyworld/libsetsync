/*
 * SetTest.cpp
 *
 *      Author: Till Lorentzen
 */

#include "SetTest.h"
#ifdef HAVE_LEVELDB
#include <setsync/storage/LevelDbStorage.h>
#endif
namespace evaluation {

SetTest::SetTest(StorageType type) {
	switch (type) {
#ifdef HAVE_LEVELDB
	/*		case LEVELDB:
			this->bfstore = new setsync::storage::LevelDbStorage();
			this->triestore = new setsync::storage::LevelDbStorage();
			this->indexstore = new setsync::storage::LevelDbStorage();
			break;*/
#endif
		case BERKELEYDB:

			break;
		default:
			this->bfstore = NULL;
			this->triestore = NULL;
			this->indexstore = NULL;
			break;
	}

}

SetTest::~SetTest() {
	delete this->bfstore;
	delete this->triestore;
	delete this->indexstore;
}

void SetTest::run(){

}

}
