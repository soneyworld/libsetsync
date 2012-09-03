/*
 * LocalSetTest.h
 *
 *      Author: Till Lorentzen
 */

#ifndef LOCALSETTEST_H_
#define LOCALSETTEST_H_
#include <cppunit/extensions/HelperMacros.h>
#include "setsync/Set.hpp"
#include "setsync/MemSet.hpp"


class MemSetTest : public CPPUNIT_NS::TestCase{

CPPUNIT_TEST_SUITE(MemSetTest);
CPPUNIT_TEST(testAddingAndErasingElements);
CPPUNIT_TEST_SUITE_END();

public:
	void setUp(void);
	void tearDown(void);
private:
	setsync::MemSet * memset;
	char * key_1;

protected:
	void testAddingAndErasingElements();

};
CPPUNIT_TEST_SUITE_REGISTRATION(MemSetTest);
#endif /* LOCALSETTEST_H_ */
