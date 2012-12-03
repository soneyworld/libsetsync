/*
 * KeyValueCountingBloomFilterTest.h
 *
 *      Author: Till Lorentzen
 */
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include <setsync/bloom/KeyValueCountingBloomFilter.h>
#include <setsync/utils/CryptoHash.h>
#include <setsync/storage/LevelDbStorage.h>
#include <setsync/storage/KeyValueStorage.h>

#ifndef KEYVALUECOUNTINGBLOOMFILTERTEST_H_
#define KEYVALUECOUNTINGBLOOMFILTERTEST_H_

namespace bloom {

class KeyValueCountingBloomFilterTest: public CppUnit::TestFixture {

CPPUNIT_TEST_SUITE(KeyValueCountingBloomFilterTest);
		CPPUNIT_TEST(testConstructor);
		CPPUNIT_TEST(testLoad);
		CPPUNIT_TEST(testInsert);
		CPPUNIT_TEST(testRemove);
		CPPUNIT_TEST(testContains);
		CPPUNIT_TEST(testContainsAll);
		CPPUNIT_TEST(testDiff);
		CPPUNIT_TEST(testToString);
	CPPUNIT_TEST_SUITE_END();

private:
	std::string path1;
	std::string path2;
	std::string path3;
	setsync::storage::AbstractKeyValueStorage * storage1;
	setsync::storage::AbstractKeyValueStorage * storage2;
	setsync::storage::AbstractKeyValueStorage * storage3;
	utils::CryptoHash hashFunction_;
public:
	void testConstructor();
	void testLoad();
	void testInsert();
	void testRemove();
	void testContains();
	void testContainsAll();
	void testDiff();
	void testToString();

	void setUp();
	void tearDown();
};
CPPUNIT_TEST_SUITE_REGISTRATION( KeyValueCountingBloomFilterTest);
}

#endif /* KEYVALUECOUNTINGBLOOMFILTERTEST_H_ */
