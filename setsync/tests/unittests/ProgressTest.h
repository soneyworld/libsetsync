/*
 * ProgressTest.h
 *
 *      Author: Till Lorentzen
 */

#ifndef PROGRESSTEST_H_
#define PROGRESSTEST_H_
#include <cppunit/extensions/HelperMacros.h>
#include <setsync/sync/Progress.h>
namespace setsync {

namespace sync {

class ProgressTest: public CPPUNIT_NS::TestCase {
CPPUNIT_TEST_SUITE( ProgressTest);
		CPPUNIT_TEST( testConstructor);
		CPPUNIT_TEST( testMinimum);
		CPPUNIT_TEST( testMaximum);
		CPPUNIT_TEST( testValue);
		CPPUNIT_TEST( testDescription);
		CPPUNIT_TEST( testPercentage);
	CPPUNIT_TEST_SUITE_END();
protected:
	void testConstructor();
	void testMinimum();
	void testMaximum();
	void testValue();
	void testDescription();
	void testPercentage();

};
CPPUNIT_TEST_SUITE_REGISTRATION(ProgressTest);
}

}

#endif /* PROGRESSTEST_H_ */
