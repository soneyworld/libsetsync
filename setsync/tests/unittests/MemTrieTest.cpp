/*
 * MemTrieTest.cpp
 *
 *      Author: Till Lorentzen
 */

#include "MemTrieTest.h"
#include <setsync/sha1.h>
#include <iostream>

using namespace std;

void MemTrieTest::setUp(void) {
	this->memtrie = new trie::MemTrie();
	this->key_1 = new unsigned char[20];
	SHA1((const unsigned char*)"hello",5,this->key_1);
	this->key_2 = new unsigned char[20];
	SHA1((const unsigned char*)"byebye",6,this->key_2);
}

void MemTrieTest::tearDown(void) {
	delete this->memtrie;
	delete this->key_1;
	delete this->key_2;
}

void MemTrieTest::testAddingAndErasingElements() {
	CPPUNIT_ASSERT(memtrie->getSize() == 0);
	memtrie->add(key_1);
	CPPUNIT_ASSERT(memtrie->getSize() == 1);
	memtrie->remove(key_1);
	CPPUNIT_ASSERT(memtrie->getSize() == 0);
	memtrie->add(key_1);
	CPPUNIT_ASSERT(memtrie->getSize() == 1);
}

void MemTrieTest::testAddingAndCleaningElements() {
	CPPUNIT_ASSERT(memtrie->getSize() == 0);
	bool result = memtrie->add(key_1);
	CPPUNIT_ASSERT(memtrie->getSize() == 1);
	CPPUNIT_ASSERT(result == true);
	// Add it again
	result = memtrie->add(key_1);
	CPPUNIT_ASSERT(memtrie->getSize() == 1);
	CPPUNIT_ASSERT(result == false);
	result = memtrie->add(key_2);
	CPPUNIT_ASSERT(memtrie->getSize() == 2);
	CPPUNIT_ASSERT(result == true);
	memtrie->clear();
	CPPUNIT_ASSERT(memtrie->getSize() == 0);
}

