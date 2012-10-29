/*
 * MemSetTest.h
 *
 *      Author: Till Lorentzen
 */

#ifndef MEMSETTEST_H_
#define MEMSETTEST_H_
#include <cppunit/extensions/HelperMacros.h>
#include "setsync/Set.hpp"
#include "setsync/MemSet.h"

namespace setsync {
class MemSetTest : public CPPUNIT_NS::TestCase{

CPPUNIT_TEST_SUITE(MemSetTest);
CPPUNIT_TEST(testAddingAndErasingElements);
CPPUNIT_TEST(testAddingAndCleaningElements);
CPPUNIT_TEST_SUITE_END();

public:
	void setUp(void);
	void tearDown(void);
private:
	setsync::MemSet * memset;
	unsigned char * key_1;

protected:
	void testAddingAndErasingElements();
	void testAddingAndCleaningElements();

};
//CPPUNIT_TEST_SUITE_REGISTRATION(MemSetTest);
};
#endif /* MEMSETTEST_H_ */
