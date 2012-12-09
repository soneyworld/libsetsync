/*
 * KeyValueIndexTest.cpp
 *
 *      Author: Till Lorentzen
 */

#include "KeyValueIndexTest.h"
#include <setsync/utils/FileSystem.h>
#ifdef HAVE_LEVELDB
#include <setsync/storage/LevelDbStorage.h>
#endif
using namespace std;

namespace setsync {

namespace index {
void KeyValueIndexTest::setUp(void) {
	path = "index";
#ifdef HAVE_LEVELDB
	this->storage = new setsync::storage::LevelDbStorage(path);
#else
	this->db = new Db(NULL, 0);
	db->open(NULL, "table1.db", NULL, DB_HASH, DB_CREATE, 0);
	this->storage = new setsync::storage::BdbStorage(this->db);
#endif

	index = new KeyValueIndex(hash, *storage);
}

void KeyValueIndexTest::tearDown(void) {
	delete this->index;
	delete this->storage;
#ifdef HAVE_LEVELDB
	utils::FileSystem::rmDirRecursive(path);
#else
	this->db->close(0);
	delete this->db;
	this->db = new Db(NULL, 0);
	db->remove("table1.db", NULL, 0);
	delete this->db;
#endif

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
