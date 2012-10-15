/*
 * DBTrieTest.h
 *
 *      Author: Till Lorentzen
 */

#ifndef FSTRIETEST_H_
#define FSTRIETEST_H_
#include <cppunit/extensions/HelperMacros.h>
#include "setsync/trie/DBTrie.h"

namespace trie {

class DbTrieTest: public CPPUNIT_NS::TestCase {

CPPUNIT_TEST_SUITE( DbTrieTest);
		CPPUNIT_TEST( testAddingAndErasingElements);
		CPPUNIT_TEST( testAddingAndCleaningElements);
		CPPUNIT_TEST( testAdding);
		CPPUNIT_TEST( testContains);
		CPPUNIT_TEST( testSize);
		CPPUNIT_TEST( testEquals);
		CPPUNIT_TEST( testSavingAndLoading);
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp(void);
	void tearDown(void);
private:
	Db * db;
	Db * db2;
protected:
	void testAdding();
	void testAddingAndErasingElements();
	void testAddingAndCleaningElements();
	void testContains();
	void testSize();
	void testEquals();
	void testSavingAndLoading();

};
CPPUNIT_TEST_SUITE_REGISTRATION(DbTrieTest);

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
		CPPUNIT_TEST( testToDb);
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp(void);
	void tearDown(void);
private:
	Db * db;
	Db * db2;
	unsigned char key1[HASHSIZE];
	unsigned char key2[HASHSIZE];
	unsigned char key3[HASHSIZE];
	unsigned char key4[HASHSIZE];
	unsigned char smaller[HASHSIZE];
	unsigned char larger[HASHSIZE];
	unsigned char hash[HASHSIZE];
protected:
	void testConstructor();
	void testEquals();
	void testNotEquals();
	void testLarger();
	void testUpdateHash();
	void testInsert();
	void testErase();
	void testCommon();
	void testToDb();
};
CPPUNIT_TEST_SUITE_REGISTRATION(DbNodeTest);

class DbRootNodeTest: public CPPUNIT_NS::TestCase {

CPPUNIT_TEST_SUITE( DbRootNodeTest);
		CPPUNIT_TEST( testConstructor);
		CPPUNIT_TEST( testToDb);
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp(void);
	void tearDown(void);
private:
	Db * db;
	Db * db2;
	unsigned char key1[HASHSIZE];
	unsigned char key2[HASHSIZE];
	unsigned char key3[HASHSIZE];
	unsigned char key4[HASHSIZE];
	unsigned char hash[HASHSIZE];
protected:
	void testConstructor();
	void testToDb();
};
CPPUNIT_TEST_SUITE_REGISTRATION(DbRootNodeTest);

}//end namespace trie
#endif /* FSTRIETEST_H_ */
