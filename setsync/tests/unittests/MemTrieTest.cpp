/*
 * MemTrieTest.cpp
 *
 *      Author: Till Lorentzen
 */

#include "MemTrieTest.h"
#include <iostream>

using namespace std;
namespace setsync {
void MemTrieTest::setUp(void) {
	this->memtrie = new trie::MemTrie(hash);
	this->key_1 = new unsigned char[hash.getHashSize()];
	hash(this->key_1, "hello");
	this->key_2 = new unsigned char[hash.getHashSize()];
	hash(this->key_2, "byebye");
}

void MemTrieTest::tearDown(void) {
	delete this->memtrie;
	delete this->key_1;
	delete this->key_2;
}

void MemTrieTest::testAddingAndErasingElements() {
	CPPUNIT_ASSERT(memtrie->getSize() == 0);
	memtrie->Trie::add(key_1);
	CPPUNIT_ASSERT(memtrie->getSize() == 1);
	memtrie->Trie::remove(key_1);
	CPPUNIT_ASSERT(memtrie->getSize() == 0);
	memtrie->Trie::add(key_1);
	CPPUNIT_ASSERT(memtrie->getSize() == 1);
}

void MemTrieTest::testAddingAndCleaningElements() {
	CPPUNIT_ASSERT(memtrie->getSize() == 0);
	bool result = memtrie->Trie::add(key_1);
	CPPUNIT_ASSERT(memtrie->getSize() == 1);
	CPPUNIT_ASSERT(result == true);
	// Add it again
	result = memtrie->Trie::add(key_1);
	CPPUNIT_ASSERT(memtrie->getSize() == 1);
	CPPUNIT_ASSERT(result == false);
	result = memtrie->Trie::add(key_2);
	CPPUNIT_ASSERT(memtrie->getSize() == 2);
	CPPUNIT_ASSERT(result == true);
	memtrie->clear();
	CPPUNIT_ASSERT(memtrie->getSize() == 0);
}
};
