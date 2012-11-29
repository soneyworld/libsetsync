/*
 * DBTrieTest.h
 *
 *      Author: Till Lorentzen
 */

#ifndef BDBTRIETEST_H_
#define BDBTRIETEST_H_
#include <cppunit/extensions/HelperMacros.h>
#include "setsync/trie/BdbTrie.h"
#include <setsync/utils/CryptoHash.h>

namespace trie {

class BdbTrieTest: public CPPUNIT_NS::TestCase {

CPPUNIT_TEST_SUITE( BdbTrieTest);
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
	Db * db;
	Db * db2;
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
CPPUNIT_TEST_SUITE_REGISTRATION(BdbTrieTest);

class DbNodeTest: public CPPUNIT_NS::TestCase {

CPPUNIT_TEST_SUITE( DbNodeTest);
		CPPUNIT_TEST( testConstructor);
		CPPUNIT_TEST( testEquals);
		CPPUNIT_TEST( testNotEquals);
		CPPUNIT_TEST( testLarger);
		CPPUNIT_TEST( testUpdateHash);
		CPPUNIT_TEST( testInsert);
		CPPUNIT_TEST( testErase);
		CPPUNIT_TEST( testCommon);
		CPPUNIT_TEST( testSimilar);
		CPPUNIT_TEST( testToDb);
		CPPUNIT_TEST( testToString);
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp(void);
	void tearDown(void);
private:
	Db * db;
	Db * db2;
	utils::CryptoHash hashFunction_;
	BdbTrie * trie;
	unsigned char * key1;
	unsigned char * key2;
	unsigned char * key3;
	unsigned char * key4;
	unsigned char * smaller;
	unsigned char * larger;
	unsigned char * hash;
protected:
	void testConstructor();
	void testEquals();
	void testNotEquals();
	void testLarger();
	void testUpdateHash();
	void testInsert();
	void testErase();
	void testCommon();
	void testSimilar();
	void testToDb();
	void testToString();
};
CPPUNIT_TEST_SUITE_REGISTRATION(DbNodeTest);

class DbRootNodeTest: public CPPUNIT_NS::TestCase {

CPPUNIT_TEST_SUITE( DbRootNodeTest);
		//		CPPUNIT_TEST( testConstructor);
		//		CPPUNIT_TEST( testToDb);
		CPPUNIT_TEST( testToString);
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp(void);
	void tearDown(void);
private:
	Db * db;
	Db * db2;
	utils::CryptoHash hash;
protected:
	void testConstructor();
	void testToDb();
	void testToString();
};
CPPUNIT_TEST_SUITE_REGISTRATION(DbRootNodeTest);

}//end namespace trie
#endif /* BDBTRIETEST_H_ */
