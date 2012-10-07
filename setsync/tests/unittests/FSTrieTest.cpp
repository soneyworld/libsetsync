/*
 * FSTrieTest.cpp
 *
 *      Author: Till Lorentzen
 */

#include "FSTrieTest.h"

void FSTrieTest::setUp(void) {
	this->fstrie = new trie::FSTrie();
	this->key_1 = new unsigned char[20];
}

void FSTrieTest::tearDown(void) {
	delete this->fstrie;
	delete this->key_1;
}

void FSTrieTest::testAdding(){
	trie::FSTrie trie;
	CPPUNIT_ASSERT(trie.getSize() == 0);
	CPPUNIT_ASSERT(trie.Trie::add("bla1"));
	CPPUNIT_ASSERT(trie.getSize() == 1);
	CPPUNIT_ASSERT(!trie.Trie::add("bla1"));
	CPPUNIT_ASSERT(trie.getSize() == 1);
	CPPUNIT_ASSERT(trie.Trie::add("bla2"));
	CPPUNIT_ASSERT(trie.getSize() == 2);
}

void FSTrieTest::testAddingAndErasingElements() {
	CPPUNIT_ASSERT(fstrie->getSize() == 0);
	fstrie->add(key_1);
	CPPUNIT_ASSERT(fstrie->getSize() == 1);
	fstrie->remove(key_1);
	CPPUNIT_ASSERT(fstrie->getSize() == 0);
	fstrie->add(key_1);
	CPPUNIT_ASSERT(fstrie->getSize() == 1);
}

void FSTrieTest::testAddingAndCleaningElements() {
	CPPUNIT_ASSERT(fstrie->getSize() == 0);
	fstrie->add(key_1);
	CPPUNIT_ASSERT(fstrie->getSize() == 1);
	fstrie->clear();
	CPPUNIT_ASSERT(fstrie->getSize() == 0);
}
