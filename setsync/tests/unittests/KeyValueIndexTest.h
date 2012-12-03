/*
 * KeyValueIndexTest.h
 *
 *      Author: Till Lorentzen
 */

#ifndef KEYVALUEINDEXTEST_H_
#define KEYVALUEINDEXTEST_H_
#include <cppunit/extensions/HelperMacros.h>
#include <setsync/utils/CryptoHash.h>
#include <setsync/index/KeyValueIndex.h>
#include <setsync/storage/KeyValueStorage.h>
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
