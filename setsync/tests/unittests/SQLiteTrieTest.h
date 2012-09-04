/*
 * SQLiteTrieTest.h
 *
 *      Author: Till Lorentzen
 */

#ifndef SQLITETRIETEST_H_
#define SQLITETRIETEST_H_
#include <cppunit/extensions/HelperMacros.h>
#include "setsync/trie/SQLiteTrie.h"

class SQLiteTrieTest: public CPPUNIT_NS::TestCase {
	CPPUNIT_TEST_SUITE( SQLiteTrieTest);
	CPPUNIT_TEST( testAddingAndErasingElements);
	CPPUNIT_TEST( testAddingAndCleaningElements);
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp(void);
	void tearDown(void);
private:
	trie::SQLiteTrie * sqltrie;
	unsigned char * key_1;
protected:
	void testAddingAndErasingElements();
	void testAddingAndCleaningElements();
};
CPPUNIT_TEST_SUITE_REGISTRATION( SQLiteTrieTest);
#endif /* SQLITETRIETEST_H_ */
