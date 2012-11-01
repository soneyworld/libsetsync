/*
 * SQLiteTrieTest.cpp
 *
 *      Author: Till Lorentzen
 */

#include "SQLiteTrieTest.h"
namespace trie {
void SQLiteTrieTest::setUp(void) {
	this->sqltrie = new trie::SQLiteTrie(hash);
	this->key_1 = new unsigned char[hash.getHashSize()];
}

void SQLiteTrieTest::tearDown(void) {
	delete this->sqltrie;
	delete this->key_1;
}

void SQLiteTrieTest::testAddingAndErasingElements() {
	CPPUNIT_ASSERT(sqltrie->getSize() == 0);
	sqltrie->add(key_1);
	CPPUNIT_ASSERT(sqltrie->getSize() == 1);
	sqltrie->remove(key_1);
	CPPUNIT_ASSERT(sqltrie->getSize() == 0);
	sqltrie->add(key_1);
	CPPUNIT_ASSERT(sqltrie->getSize() == 1);
}

void SQLiteTrieTest::testAddingAndCleaningElements() {
	CPPUNIT_ASSERT(sqltrie->getSize() == 0);
	sqltrie->add(key_1);
	CPPUNIT_ASSERT(sqltrie->getSize() == 1);
	sqltrie->clear();
	CPPUNIT_ASSERT(sqltrie->getSize() == 0);
}
};
