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
	CPPUNIT_TEST( testStartSync);
	CPPUNIT_TEST( testLooseSync);
	CPPUNIT_TEST( testStrictSync);
	CPPUNIT_TEST( testSync);
	CPPUNIT_TEST( testCAPI);
CPPUNIT_TEST_SUITE_END();

private:
	utils::FileSystem::TemporaryDirectory * dir;
	config::Configuration config;
	config::Configuration config2;

public:
	void setUp(void);
	void tearDown(void);
protected:
	void testInsert();
	void testErase();
	void testClear();
	void testFind();
	void testMaximum();
	void testStartSync();
	void testLooseSync();
	void testStrictSync();
	void testSync();
	void testCAPI();
};
CPPUNIT_TEST_SUITE_REGISTRATION( SetTest);
};
#endif /* SETTEST_H_ */
