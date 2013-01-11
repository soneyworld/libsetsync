/*
 * KeyValueTrieTest.h
 *
 *      Author: Till Lorentzen
 */

#ifndef KEYVALUETRIETEST_H_
#define KEYVALUETRIETEST_H_
#include <cppunit/extensions/HelperMacros.h>
#include <setsync/trie/KeyValueTrie.h>
#include <setsync/crypto/CryptoHash.h>
#include <setsync/storage/BdbStorage.h>
namespace setsync {
namespace trie {


class KeyValueTrieTest: public CPPUNIT_NS::TestCase {
CPPUNIT_TEST_SUITE( KeyValueTrieTest);
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
	setsync::storage::BdbStorage * storage1;
	setsync::storage::BdbStorage * storage2;
	crypto::CryptoHash hash;
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
CPPUNIT_TEST_SUITE_REGISTRATION(KeyValueTrieTest);

}
}
#endif /* KEYVALUETRIETEST_H_ */
