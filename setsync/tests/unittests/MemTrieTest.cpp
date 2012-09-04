/*
 * MemTrieTest.cpp
 *
 *      Author: Till Lorentzen
 */

#include "MemTrieTest.h"

void MemTrieTest::setUp(void) {
	this->memtrie = new trie::MemTrie();
	this->key_1 = new unsigned char[20];
}

void MemTrieTest::tearDown(void) {
	delete this->memtrie;
	delete this->key_1;
}

void MemTrieTest::testAddingAndErasingElements() {
	CPPUNIT_ASSERT(memtrie->getSize() == 0);
	memtrie->insert(key_1);
	CPPUNIT_ASSERT(memtrie->getSize() == 1);
	memtrie->erase(key_1);
	CPPUNIT_ASSERT(memtrie->getSize() == 0);
	memtrie->insert(key_1);
	CPPUNIT_ASSERT(memtrie->getSize() == 1);
}

void MemTrieTest::testAddingAndCleaningElements() {
	CPPUNIT_ASSERT(memtrie->getSize() == 0);
	memtrie->insert(key_1);
	CPPUNIT_ASSERT(memtrie->getSize() == 1);
	memtrie->clear();
	CPPUNIT_ASSERT(memtrie->getSize() == 0);
}
