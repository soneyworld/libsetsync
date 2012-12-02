/*
 * LevelDbStorageTest.h
 *
 *      Author: Till Lorentzen
 */
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include <setsync/storage/LevelDbStorage.h>
#include <setsync/utils/CryptoHash.h>

#ifndef LEVELDBSTORAGETEST_H_
#define LEVELDBSTORAGETEST_H_

namespace setsync {

namespace storage {

class LevelDbStorageTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(LevelDbStorageTest);
		CPPUNIT_TEST(testPut);
		CPPUNIT_TEST(testGet);
		CPPUNIT_TEST(testDelete);
		CPPUNIT_TEST(testClear);
	CPPUNIT_TEST_SUITE_END();

private:
	utils::CryptoHash hash;
	LevelDbStorage * storage;
public:
	void setUp();
	void tearDown();
	void testPut();
	void testGet();
	void testDelete();
	void testClear();

};

CPPUNIT_TEST_SUITE_REGISTRATION( LevelDbStorageTest);
}

}

#endif /* LEVELDBSTORAGETEST_H_ */
