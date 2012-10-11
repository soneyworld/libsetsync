/*
 * DBTrieTest.h
 *
 *      Author: Till Lorentzen
 */

#ifndef FSTRIETEST_H_
#define FSTRIETEST_H_
#include <cppunit/extensions/HelperMacros.h>
#include "setsync/trie/DBTrie.h"


class DBTrieTest: public CPPUNIT_NS::TestCase {

	CPPUNIT_TEST_SUITE( DBTrieTest);
	CPPUNIT_TEST( testAddingAndErasingElements);
	CPPUNIT_TEST( testAddingAndCleaningElements);
	CPPUNIT_TEST( testAdding);
	CPPUNIT_TEST( testContains);
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp(void);
	void tearDown(void);
private:
	Db * db;
protected:
	void testAdding();
	void testAddingAndErasingElements();
	void testAddingAndCleaningElements();
	void testContains();

};
CPPUNIT_TEST_SUITE_REGISTRATION(DBTrieTest);
#endif /* FSTRIETEST_H_ */
