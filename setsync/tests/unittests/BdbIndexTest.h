/*
 * BdbIndexTest.h
 *
 *      Author: Till Lorentzen
 */

#ifndef DBINDEXTEST_H_
#define DBINDEXTEST_H_
#include <cppunit/extensions/HelperMacros.h>
#include <setsync/utils/CryptoHash.h>
#include <setsync/index/BdbIndex.h>

namespace setsync {

namespace index {

class DbIndexTest: public CPPUNIT_NS::TestCase {

CPPUNIT_TEST_SUITE( DbIndexTest);
		CPPUNIT_TEST( testAdding);
	CPPUNIT_TEST_SUITE_END();
public:
	void setUp(void);
	void tearDown(void);
private:
	Db * db;
	utils::CryptoHash hash;
	DbIndex * index;
protected:
	void testAdding();
};

CPPUNIT_TEST_SUITE_REGISTRATION(DbIndexTest);

}

}

#endif /* DBINDEXTEST_H_ */
