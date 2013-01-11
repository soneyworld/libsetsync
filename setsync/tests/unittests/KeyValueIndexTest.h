/*
 * KeyValueIndexTest.h
 *
 *      Author: Till Lorentzen
 */

#ifndef KEYVALUEINDEXTEST_H_
#define KEYVALUEINDEXTEST_H_
#include <cppunit/extensions/HelperMacros.h>
#include <setsync/crypto/CryptoHash.h>
#include <setsync/index/KeyValueIndex.h>
#include <setsync/storage/KeyValueStorage.h>
#ifdef HAVE_DB_CXX_H
#include <setsync/storage/BdbStorage.h>
#endif
namespace setsync {

namespace index {

class KeyValueIndexTest: public CPPUNIT_NS::TestCase {
CPPUNIT_TEST_SUITE( KeyValueIndexTest);
		CPPUNIT_TEST( testAdding);
		CPPUNIT_TEST( testClean);
	CPPUNIT_TEST_SUITE_END();
public:
	void setUp(void);
	void tearDown(void);
private:
	std::string path;
#ifndef HAVE_LEVELDB
#ifdef HAVE_DB_CXX_H
	std::string bdbpath;
	Db * db;
#endif
#endif
	setsync::storage::AbstractKeyValueStorage * storage;
	utils::CryptoHash hash;
	KeyValueIndex * index;
protected:
	void testAdding();
	void testClean();

};
CPPUNIT_TEST_SUITE_REGISTRATION(KeyValueIndexTest);
}

}

#endif /* KEYVALUEINDEXTEST_H_ */
