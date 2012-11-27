/*
 * MemSetTest.h
 *
 *      Author: Till Lorentzen
 */

#ifndef MEMSETTEST_H_
#define MEMSETTEST_H_
#include <cppunit/extensions/HelperMacros.h>
#include "setsync/Set.hpp"
#include "setsync/DbSet.h"

namespace setsync {
class DbSetTest: public CPPUNIT_NS::TestCase {

CPPUNIT_TEST_SUITE(DbSetTest);
		CPPUNIT_TEST(testAdding);
		CPPUNIT_TEST(testSize);
		CPPUNIT_TEST(testErasing);
		CPPUNIT_TEST(testAddingAndErasingElements);
		CPPUNIT_TEST(testAddingAndCleaningElements);
		CPPUNIT_TEST(testSync);
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp(void);
	void tearDown(void);
private:
	unsigned char * key_1;
	utils::CryptoHash hash;

protected:
	void testAdding();
	void testSize();
	void testErasing();
	void testAddingAndErasingElements();
	void testAddingAndCleaningElements();
	void testSync();

};
CPPUNIT_TEST_SUITE_REGISTRATION(DbSetTest);
}
;
#endif /* MEMSETTEST_H_ */
