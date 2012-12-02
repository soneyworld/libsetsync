/*
 * LevelDbStorageTest.cpp
 *
 *      Author: Till Lorentzen
 */

#include "LevelDbStorageTest.h"
#include <setsync/utils/FileSystem.h>

namespace setsync {

namespace storage {

void LevelDbStorageTest::setUp() {
	this->storage = new LevelDbStorage("leveldb-temp-db");
}
void LevelDbStorageTest::tearDown() {
	delete this->storage;
	utils::FileSystem::rmDirRecursive("leveldb-temp-db");
}
void LevelDbStorageTest::testPut() {
	unsigned char keybuffer[hash.getHashSize()];
	for (int i = 0; i < 100; i++) {
		std::stringstream ss;
		ss << "bla" << i;
		hash(keybuffer, ss.str());
		this->storage->put(keybuffer, hash.getHashSize(), NULL, 0);
	}
}
void LevelDbStorageTest::testGet() {
	unsigned char keybuffer[hash.getHashSize()];
	std::size_t valueSize = 10;
	unsigned char valuebuffer[10];
	memset(valuebuffer, 1, valueSize);
	// Put 100 keys
	for (int i = 0; i < 100; i++) {
		std::stringstream ss;
		ss << "bla" << i;
		hash(keybuffer, ss.str());
		this->storage->put(keybuffer, hash.getHashSize(), valuebuffer,
				valueSize);
	}
	// Get the same 100 keys
	for (int i = 0; i < 100; i++) {
		std::stringstream ss;
		ss << "bla" << i;
		std::size_t resultSize;
		hash(keybuffer, ss.str());
		unsigned char * result;
		CPPUNIT_ASSERT(this->storage->get(keybuffer, hash.getHashSize(), &result, &resultSize));
		CPPUNIT_ASSERT_EQUAL(valueSize, resultSize);
		CPPUNIT_ASSERT(memcmp(valuebuffer,result,resultSize)==0);
		free(result);
	}
	// Request non existing key
	hash(keybuffer, "do not exist");
	unsigned char * result;
	std::size_t resultSize;
	CPPUNIT_ASSERT(!this->storage->get(keybuffer,hash.getHashSize(),&result, &resultSize));
	CPPUNIT_ASSERT(result == NULL );
	CPPUNIT_ASSERT(resultSize == 0 );

}
void LevelDbStorageTest::testDelete() {
	unsigned char keybuffer[hash.getHashSize()];
	std::size_t valueSize = 10;
	unsigned char valuebuffer[10];
	memset(valuebuffer, 1, valueSize);
	// Put 100 keys
	for (int i = 0; i < 100; i++) {
		std::stringstream ss;
		ss << "bla" << i;
		hash(keybuffer, ss.str());
		this->storage->put(keybuffer, hash.getHashSize(), valuebuffer,
				valueSize);
	}
	// Get the same 100 keys
	for (int i = 0; i < 100; i++) {
		std::stringstream ss;
		ss << "bla" << i;
		std::size_t resultSize;
		hash(keybuffer, ss.str());
		unsigned char * result;
		CPPUNIT_ASSERT(this->storage->get(keybuffer, hash.getHashSize(), &result, &resultSize));
		CPPUNIT_ASSERT_EQUAL(valueSize, resultSize);
		CPPUNIT_ASSERT(memcmp(valuebuffer,result,resultSize)==0);
		free(result);
	}
	// Remove the same 100 keys
	for (int i = 0; i < 100; i++) {
		std::stringstream ss;
		ss << "bla" << i;
		hash(keybuffer, ss.str());
		this->storage->del(keybuffer, hash.getHashSize());
	}
	// Get the deleted 100 keys
	for (int i = 0; i < 100; i++) {
		std::stringstream ss;
		ss << "bla" << i;
		std::size_t resultSize;
		hash(keybuffer, ss.str());
		unsigned char * result;
		CPPUNIT_ASSERT(!this->storage->get(keybuffer,hash.getHashSize(),&result, &resultSize));
		CPPUNIT_ASSERT(result == NULL );
		CPPUNIT_ASSERT(resultSize == 0 );
	}
}

void LevelDbStorageTest::testClear() {
	unsigned char keybuffer[hash.getHashSize()];
	std::size_t valueSize = 10;
	unsigned char valuebuffer[10];
	memset(valuebuffer, 1, valueSize);
	// Put 100 keys
	for (int i = 0; i < 100; i++) {
		std::stringstream ss;
		ss << "bla" << i;
		hash(keybuffer, ss.str());
		this->storage->put(keybuffer, hash.getHashSize(), valuebuffer,
				valueSize);
	}
	this->storage->clear();
	// Get the deleted 100 keys
	for (int i = 0; i < 100; i++) {
		std::stringstream ss;
		ss << "bla" << i;
		std::size_t resultSize;
		hash(keybuffer, ss.str());
		unsigned char * result;
		CPPUNIT_ASSERT(!this->storage->get(keybuffer,hash.getHashSize(),&result, &resultSize));
		CPPUNIT_ASSERT(result == NULL );
		CPPUNIT_ASSERT(resultSize == 0 );
	}

}

}

}
