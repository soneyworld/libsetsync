/*
 * KeyValueStorageTest.cpp
 *
 *      Author: Till Lorentzen
 */

#include "KeyValueStorageTest.h"
#include <setsync/utils/FileSystem.h>
#include <setsync/storage/MemStorage.h>
#include <stdlib.h>
#include <string.h>

namespace setsync {

namespace storage {

void KeyValueStorageTest::setUp(void) {
#ifdef HAVE_LEVELDB
	levelpath = "temp-leveldb-unittest";
	LevelDbStorage * level = new LevelDbStorage(levelpath);
	stores.push_back(level);
#endif
#ifdef HAVE_DB_CXX_H
	bdbpath = "temp-bdb-unittest";
	this->db = new Db(NULL, 0);
	db->open(NULL, bdbpath.c_str(), "storage", DB_HASH, DB_CREATE, 0);
	BdbStorage * bdb = new BdbStorage(this->db);
	stores.push_back(bdb);
#endif
	MemStorage * mem = new MemStorage(1,0);
	stores.push_back(mem);
}
void KeyValueStorageTest::tearDown(void) {
#ifdef HAVE_LEVELDB
	delete stores.front();
	stores.pop_front();
	utils::FileSystem::rmDirRecursive(levelpath);
#endif
#ifdef HAVE_DB_CXX_H
	BdbStorage * bdb = dynamic_cast<BdbStorage *> (stores.front());
	stores.pop_front();
	delete bdb;
	db->close(0);
	delete this->db;
	this->db = new Db(NULL, 0);
	this->db->remove(bdbpath.c_str(), NULL, 0);
	delete this->db;
#endif
	delete stores.front();
	stores.pop_front();
}

void KeyValueStorageTest::testPut() {
	std::list<AbstractKeyValueStorage *>::iterator p = stores.begin();
	while (p != stores.end()) {
		AbstractKeyValueStorage * storage = *p;

		std::size_t valueSize = 10;
		unsigned char valuebuffer[10];
		memset(valuebuffer, 1, valueSize);
		unsigned char keybuffer[hash.getHashSize()];
		for (int i = 0; i < 100; i++) {
			std::stringstream ss;
			ss << "bla" << i;
			hash(keybuffer, ss.str());
			storage->put(keybuffer, hash.getHashSize(), valuebuffer, valueSize);
		}
		for (int i = 0; i < 100; i++) {
			std::stringstream ss;
			ss << "bla" << i;
			std::size_t resultSize;
			hash(keybuffer, ss.str());
			unsigned char * result;
			CPPUNIT_ASSERT(storage->get(keybuffer, hash.getHashSize(), &result, &resultSize));
			CPPUNIT_ASSERT_EQUAL(valueSize, resultSize);
			CPPUNIT_ASSERT(memcmp(valuebuffer,result,resultSize)==0);
			free(result);
		}
		p++;
	}
}
void KeyValueStorageTest::testGet() {
	std::list<AbstractKeyValueStorage *>::iterator p = stores.begin();
	while (p != stores.end()) {
		AbstractKeyValueStorage * storage = *p;
		std::size_t valueSize = 10;
		unsigned char valuebuffer[10];
		memset(valuebuffer, 1, valueSize);
		unsigned char keybuffer[hash.getHashSize()];
		for (int i = 0; i < 100; i++) {
			std::stringstream ss;
			ss << "bla" << i;
			std::size_t resultSize;
			hash(keybuffer, ss.str());
			unsigned char * result;
			CPPUNIT_ASSERT(!storage->get(keybuffer, hash.getHashSize(), &result, &resultSize));
		}
		for (int i = 0; i < 100; i++) {
			std::stringstream ss;
			ss << "bla" << i;
			hash(keybuffer, ss.str());
			storage->put(keybuffer, hash.getHashSize(), valuebuffer, valueSize);
		}
		for (int i = 0; i < 100; i++) {
			std::stringstream ss;
			ss << "bla" << i;
			std::size_t resultSize;
			hash(keybuffer, ss.str());
			unsigned char * result;
			CPPUNIT_ASSERT(storage->get(keybuffer, hash.getHashSize(), &result, &resultSize));
			CPPUNIT_ASSERT_EQUAL(valueSize, resultSize);
			CPPUNIT_ASSERT(memcmp(valuebuffer,result,resultSize)==0);
			free(result);
		}
		p++;
	}
}
void KeyValueStorageTest::testDelete() {
	std::list<AbstractKeyValueStorage *>::iterator p = stores.begin();
	while (p != stores.end()) {
		AbstractKeyValueStorage * storage = *p;
		std::size_t valueSize = 10;
		unsigned char valuebuffer[10];
		memset(valuebuffer, 1, valueSize);
		unsigned char keybuffer[hash.getHashSize()];
		for (int i = 0; i < 100; i++) {
			std::stringstream ss;
			ss << "bla" << i;
			hash(keybuffer, ss.str());
			storage->put(keybuffer, hash.getHashSize(), valuebuffer, valueSize);
		}
		for (int i = 0; i < 100; i++) {
			std::stringstream ss;
			ss << "bla" << i;
			hash(keybuffer, ss.str());
			storage->del(keybuffer, hash.getHashSize());
		}
		for (int i = 0; i < 100; i++) {
			std::stringstream ss;
			ss << "bla" << i;
			std::size_t resultSize;
			hash(keybuffer, ss.str());
			unsigned char * result;
			CPPUNIT_ASSERT(!storage->get(keybuffer, hash.getHashSize(), &result, &resultSize));
		}
		p++;
	}
}
void KeyValueStorageTest::testIterator() {
	std::list<AbstractKeyValueStorage *>::iterator p = stores.begin();
	while (p != stores.end()) {
		AbstractKeyValueStorage * storage = *p;
		std::size_t valueSize = 10;
		unsigned char valuebuffer[10];
		memset(valuebuffer, 1, valueSize);
		unsigned char keybuffer[hash.getHashSize()];
		for (int i = 0; i < 100; i++) {
			std::stringstream ss;
			ss << "bla" << i;
			hash(keybuffer, ss.str());
			storage->put(keybuffer, hash.getHashSize(), valuebuffer, valueSize);
		}
		AbstractKeyValueIterator * iterator = storage->createIterator();
		std::size_t resultSize;
		unsigned char * result;
		iterator->seekToFirst();
		while (iterator->valid()) {
			CPPUNIT_ASSERT(iterator->keySize() == hash.getHashSize());
			iterator->key(keybuffer);
			CPPUNIT_ASSERT(storage->get(keybuffer,iterator->keySize(),&result, &resultSize));
			CPPUNIT_ASSERT_EQUAL(valueSize, resultSize);
			CPPUNIT_ASSERT(memcmp(valuebuffer,result,resultSize)==0);
			free(result);
			iterator->next();
		}
		delete iterator;
		p++;
	}
}
void KeyValueStorageTest::testClear() {

	std::list<AbstractKeyValueStorage *>::iterator p = stores.begin();
	while (p != stores.end()) {
		AbstractKeyValueStorage * storage = *p;
		std::size_t valueSize = 10;
		unsigned char valuebuffer[10];
		memset(valuebuffer, 1, valueSize);
		unsigned char keybuffer[hash.getHashSize()];
		for (int i = 0; i < 100; i++) {
			std::stringstream ss;
			ss << "bla" << i;
			hash(keybuffer, ss.str());
			storage->put(keybuffer, hash.getHashSize(), valuebuffer, valueSize);
		}
		storage->clear();
		for (int i = 0; i < 100; i++) {
			std::stringstream ss;
			ss << "bla" << i;
			std::size_t resultSize;
			hash(keybuffer, ss.str());
			unsigned char * result;
			CPPUNIT_ASSERT(!storage->get(keybuffer, hash.getHashSize(), &result, &resultSize));
		}
		p++;
	}

}

}

}
