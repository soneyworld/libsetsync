/*
 * SetTest.h
 *
 *      Author: Till Lorentzen
 */

#ifndef SETTEST_H_
#define SETTEST_H_
#include <cppunit/extensions/HelperMacros.h>
#include <setsync/Set.hpp>
#include <setsync/utils/FileSystem.h>

namespace setsync {
class SetTest: public CPPUNIT_NS::TestCase {


CPPUNIT_TEST_SUITE( SetTest);
	CPPUNIT_TEST( testInsert);
	CPPUNIT_TEST( testErase);
	CPPUNIT_TEST( testClear);
	CPPUNIT_TEST( testFind);
	CPPUNIT_TEST( testMaximum);
	CPPUNIT_TEST( testSync);
CPPUNIT_TEST_SUITE_END();

private:
	utils::FileSystem::TemporaryDirectory * dir;
	config::Configuration config;

public:
	void setUp(void);
	void tearDown(void);
protected:
	void testInsert();
	void testErase();
	void testClear();
	void testFind();
	void testMaximum();
	void testSync();
};
CPPUNIT_TEST_SUITE_REGISTRATION( SetTest);
};
#endif /* SETTEST_H_ */
