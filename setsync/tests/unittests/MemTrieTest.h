/*
 * MemTrieTest.h
 *
 *      Author: Till Lorentzen
 */

#ifndef MEMTRIETEST_H_
#define MEMTRIETEST_H_
#include <cppunit/extensions/HelperMacros.h>
#include "setsync/trie/MemTrie.h"

class MemTrieTest: public CPPUNIT_NS::TestCase {

	CPPUNIT_TEST_SUITE( MemTrieTest);
	CPPUNIT_TEST( testAddingAndErasingElements);
	CPPUNIT_TEST( testAddingAndCleaningElements);
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp(void);
	void tearDown(void);
private:
	trie::MemTrie * memtrie;
	unsigned char * key_1;
protected:
	void testAddingAndErasingElements();
	void testAddingAndCleaningElements();
};
CPPUNIT_TEST_SUITE_REGISTRATION(MemTrieTest);
#endif /* MEMTRIETEST_H_ */
