/*
 * DiffHandlerTest.h
 *
 *      Author: Till Lorentzen
 */

#ifndef DIFFHANDLERTEST_H_
#define DIFFHANDLERTEST_H_
#include <cppunit/extensions/HelperMacros.h>
#include <setsync/DiffHandler.h>
#include <setsync/crypto/CryptoHash.h>

namespace setsync {

class DiffHandlerTest: public CPPUNIT_NS::TestCase {
CPPUNIT_TEST_SUITE( DiffHandlerTest);
		CPPUNIT_TEST( testListHandle );
		CPPUNIT_TEST( testOutputStreamHandle );
		CPPUNIT_TEST( testC_Handle );
		CPPUNIT_TEST( testFilteredHandle );
	CPPUNIT_TEST_SUITE_END();

	friend void callback(void *closure, const unsigned char * hash,
			const size_t hashsize, const size_t existsLocally);

private:
	crypto::CryptoHash hash_;
	unsigned int counts;
public:
	void setUp(void) {
		counts = 0;
	}
	void tearDown(void) {
	}
protected:
	void testListHandle();
	void testOutputStreamHandle();
	void testC_Handle();
	void testFilteredHandle();
};
CPPUNIT_TEST_SUITE_REGISTRATION(DiffHandlerTest);
}

#endif /* DIFFHANDLERTEST_H_ */
