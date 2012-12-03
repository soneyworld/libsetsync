/*
 * LevelDbTrieTest.h
 *
 *      Author: Till Lorentzen
 */

#ifndef LEVELDBTRIETEST_H_
#define LEVELDBTRIETEST_H_
#include <cppunit/extensions/HelperMacros.h>
#include <setsync/trie/LevelDbTrie.h>
#include <setsync/storage/KeyValueStorage.h>
#include <setsync/utils/CryptoHash.h>
namespace trie {

class LevelDbTrieTest: public CPPUNIT_NS::TestCase {

CPPUNIT_TEST_SUITE( LevelDbTrieTest);
		CPPUNIT_TEST( testAddingAndErasingElements);
		CPPUNIT_TEST( testAddingAndCleaningElements);
		CPPUNIT_TEST( testAdding);
		CPPUNIT_TEST( testContains);
		CPPUNIT_TEST( testSize);
		CPPUNIT_TEST( testEquals);
		CPPUNIT_TEST( testSavingAndLoading);
		CPPUNIT_TEST( testToString);
		CPPUNIT_TEST( testSubTrie);
		CPPUNIT_TEST( testDiff);
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp(void);
	void tearDown(void);
private:
	std::string path1;
	std::string path2;
	setsync::storage::LevelDbStorage * storage1;
	setsync::storage::LevelDbStorage * storage2;
	utils::CryptoHash hash;
	unsigned char * smaller;
	unsigned char * larger;
protected:
	void testAdding();
	void testAddingAndErasingElements();
	void testAddingAndCleaningElements();
	void testContains();
	void testSize();
	void testEquals();
	void testSavingAndLoading();
	void testToString();
	void testSubTrie();
	void testDiff();
};
CPPUNIT_TEST_SUITE_REGISTRATION(LevelDbTrieTest);
}

#endif /* LEVELDBTRIETEST_H_ */
