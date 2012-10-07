/*
 * FSTrieTest.cpp
 *
 *      Author: Till Lorentzen
 */

#include "DBTrieTest.h"

void DBTrieTest::setUp(void) {
}

void DBTrieTest::tearDown(void) {
}

void DBTrieTest::testAdding() {
	trie::DBTrie trie;
	CPPUNIT_ASSERT(trie.getSize() == 0);
	CPPUNIT_ASSERT(trie.Trie::add("bla1"));
	CPPUNIT_ASSERT(trie.getSize() == 1);
	CPPUNIT_ASSERT(!trie.Trie::add("bla1"));
	CPPUNIT_ASSERT(trie.getSize() == 1);
	CPPUNIT_ASSERT(trie.Trie::add("bla2"));
	CPPUNIT_ASSERT(trie.getSize() == 2);
}

void DBTrieTest::testAddingAndErasingElements() {
	trie::DBTrie trie;
	CPPUNIT_ASSERT(trie.getSize() == 0);
	CPPUNIT_ASSERT(trie.Trie::add("bla1"));
	CPPUNIT_ASSERT(trie.getSize() == 1);
	CPPUNIT_ASSERT(trie.Trie::remove("bla1"));
	CPPUNIT_ASSERT(trie.getSize() == 0);
	CPPUNIT_ASSERT(!trie.Trie::remove("bla1"));
	CPPUNIT_ASSERT(trie.getSize() == 0);
	CPPUNIT_ASSERT(trie.Trie::add("bla1"));
	CPPUNIT_ASSERT(trie.Trie::add("bla2"));
	CPPUNIT_ASSERT(trie.getSize() == 2);
	CPPUNIT_ASSERT(trie.Trie::remove("bla1"));
	CPPUNIT_ASSERT(!trie.Trie::remove("bla1"));
	CPPUNIT_ASSERT(trie.getSize() == 1);
	CPPUNIT_ASSERT(trie.Trie::remove("bla2"));
	CPPUNIT_ASSERT(trie.getSize() == 0);
	CPPUNIT_ASSERT(!trie.Trie::remove("bla2"));
	CPPUNIT_ASSERT(trie.getSize() == 0);
}

void DBTrieTest::testAddingAndCleaningElements() {
	trie::DBTrie trie;
	CPPUNIT_ASSERT(trie.getSize() == 0);
	CPPUNIT_ASSERT(trie.Trie::add("bla1"));
	CPPUNIT_ASSERT(trie.getSize() == 1);
	trie.clear();
	CPPUNIT_ASSERT(trie.getSize() == 0);
}
