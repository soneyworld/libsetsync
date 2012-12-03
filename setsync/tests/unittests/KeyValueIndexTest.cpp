/*
 * KeyValueIndexTest.cpp
 *
 *      Author: Till Lorentzen
 */

#include "KeyValueIndexTest.h"
#include <setsync/utils/FileSystem.h>
#include <setsync/storage/LevelDbStorage.h>

using namespace std;

namespace setsync {

namespace index {
void KeyValueIndexTest::setUp(void) {
	path = "index";
	this->storage = new setsync::storage::LevelDbStorage(path);
	index = new KeyValueIndex(hash, *storage);
}

void KeyValueIndexTest::tearDown(void) {
	delete this->index;
	delete this->storage;
	utils::FileSystem::rmDirRecursive(path);
}

void KeyValueIndexTest::testAdding(void) {
	unsigned char key[hash.getHashSize()];
	size_t buffersize = 0;
	unsigned char buffer[buffersize];
	hash(key, "bla1");
	this->index->put(key, buffer, 0);
	CPPUNIT_ASSERT(this->index->get(key,buffer,&buffersize));
}

void KeyValueIndexTest::testClean(void) {
	unsigned char key[hash.getHashSize()];
	hash(key, "bla1");
	this->index->put(key, NULL, 0);
	this->index->clear();
	CPPUNIT_ASSERT(!this->index->get(key,NULL,0));
}
}

}
