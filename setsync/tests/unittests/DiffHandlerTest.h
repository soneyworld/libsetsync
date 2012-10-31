/*
 * DiffHandlerTest.h
 *
 *      Author: Till Lorentzen
 */

#ifndef DIFFHANDLERTEST_H_
#define DIFFHANDLERTEST_H_
#include <cppunit/extensions/HelperMacros.h>
#include <setsync/DiffHandler.h>
#include <setsync/utils/CryptoHash.h>

namespace setsync {

class DiffHandlerTest: public CPPUNIT_NS::TestCase {
	CPPUNIT_TEST_SUITE( DiffHandlerTest);
	CPPUNIT_TEST( testListHandle );
	CPPUNIT_TEST( testOutputStreamHandle );
	CPPUNIT_TEST_SUITE_END();
public:
	void setUp(void){};
	void tearDown(void){};
protected:
	void testListHandle();
	void testOutputStreamHandle();
private:
	utils::CryptoHash hash_;
};
CPPUNIT_TEST_SUITE_REGISTRATION(DiffHandlerTest);
}

#endif /* DIFFHANDLERTEST_H_ */
