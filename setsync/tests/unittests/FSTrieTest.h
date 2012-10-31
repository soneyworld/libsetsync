/*
 * FSTrieTest.h
 *
 *      Author: Till Lorentzen
 */

#ifndef FSTRIETEST_H_
#define FSTRIETEST_H_
#include <cppunit/extensions/HelperMacros.h>
#include "setsync/trie/FSTrie.h"

namespace trie {
class FSTrieTest: public CPPUNIT_NS::TestCase {

	CPPUNIT_TEST_SUITE( FSTrieTest);
	CPPUNIT_TEST( testAddingAndErasingElements);
	CPPUNIT_TEST( testAddingAndCleaningElements);
	CPPUNIT_TEST( testAdding);
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp(void);
	void tearDown(void);
private:
	trie::FSTrie * fstrie;
	unsigned char * key_1;
	utils::CryptoHash hash;

protected:
	void testAdding();
	void testAddingAndErasingElements();
	void testAddingAndCleaningElements();

};
//CPPUNIT_TEST_SUITE_REGISTRATION(FSTrieTest);
}
#endif /* FSTRIETEST_H_ */
