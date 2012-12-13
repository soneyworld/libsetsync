/*
 * KeyValueCountingBloomFilterTest.h
 *
 *      Author: Till Lorentzen
 */
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include <setsync/bloom/KeyValueCountingBloomFilter.h>
#include <setsync/utils/CryptoHash.h>
#ifdef HAVE_LEVELDB
#include <setsync/storage/LevelDbStorage.h>
#endif
#include <setsync/storage/KeyValueStorage.h>
#include <db_cxx.h>

#ifndef KEYVALUECOUNTINGBLOOMFILTERTEST_H_
#define KEYVALUECOUNTINGBLOOMFILTERTEST_H_

namespace bloom {

class KeyValueBloomFilterSyncTest: public CppUnit::TestFixture {
CPPUNIT_TEST_SUITE(KeyValueBloomFilterSyncTest);
		CPPUNIT_TEST(testInput);
		CPPUNIT_TEST(testOutput);
	CPPUNIT_TEST_SUITE_END();
private:
	Db * db;
	KeyValueCountingBloomFilter * filter;
	setsync::storage::AbstractKeyValueStorage * storage;
	utils::CryptoHash hashFunction_;
	std::string filename_;
	setsync::sync::AbstractSyncProcessPart * process;
protected:
	void testInput();
	void testOutput();
public:
	void setUp();
	void tearDown();
};

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
	Db * db1;
#ifdef HAVE_LEVELDB
	std::string path1;
	std::string path2;
	std::string path3;
#else
	Db * db2;
	Db * db3;
	Db * db4;
#endif
	setsync::storage::AbstractKeyValueStorage * storage1;
	setsync::storage::AbstractKeyValueStorage * storage2;
	setsync::storage::AbstractKeyValueStorage * storage3;
	setsync::storage::AbstractKeyValueStorage * bdbstorage;
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
CPPUNIT_TEST_SUITE_REGISTRATION( KeyValueBloomFilterSyncTest);
CPPUNIT_TEST_SUITE_REGISTRATION( KeyValueCountingBloomFilterTest);
}

#endif /* KEYVALUECOUNTINGBLOOMFILTERTEST_H_ */
