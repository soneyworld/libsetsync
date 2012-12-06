/*
 * KeyValueStorageTest.h
 *
 *      Author: Till Lorentzen
 */

#ifndef KEYVALUESTORAGETEST_H_
#define KEYVALUESTORAGETEST_H_
#include <cppunit/extensions/HelperMacros.h>
#include <setsync/storage/KeyValueStorage.h>
#include <list>
#ifdef HAVE_LEVELDB
#include <setsync/storage/LevelDbStorage.h>
#endif
#ifdef HAVE_DB_CXX_H
#include <setsync/storage/BdbStorage.h>
#endif
#ifdef HAVE_SQLITE
//#include <setsync/storage/SqliteStorage.h>
#endif
//#include <setsync/storage/MemStorage.h>

namespace setsync {

namespace storage {

class KeyValueStorageTest: public CPPUNIT_NS::TestCase {
CPPUNIT_TEST_SUITE( KeyValueStorageTest);
		CPPUNIT_TEST( testPut);
		CPPUNIT_TEST( testGet);
		CPPUNIT_TEST( testDelete);
		CPPUNIT_TEST( testIterator);
		CPPUNIT_TEST( testClear);
	CPPUNIT_TEST_SUITE_END();
private:
	std::list<AbstractKeyValueStorage *> stores;
	utils::CryptoHash hash;
#ifdef HAVE_LEVELDB
	std::string levelpath;
#endif
#ifdef HAVE_DB_CXX_H
	std::string bdbpath;
	Db * db;
#endif
public:
	void setUp(void);
	void tearDown(void);
protected:
	void testPut();
	void testGet();
	void testDelete();
	void testIterator();
	void testClear();
};
CPPUNIT_TEST_SUITE_REGISTRATION(KeyValueStorageTest);
}

}

#endif /* KEYVALUESTORAGETEST_H_ */
