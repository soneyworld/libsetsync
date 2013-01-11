///
/// @brief       CPPUnit-Tests for class DoubleFilterScheme and ExtendedDoubleHashingScheme
/// @author      Till Lorentzen (lorentze@ibr.cs.tu-bs.de)
/// 


#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include <setsync/crypto/CryptoHash.h>

#ifndef DOUBLEFILTERSCHEMETEST_H
#define DOUBLEFILTERSCHEMETEST_H
namespace setsync {
namespace bloom {
class DoubleHashingSchemeTest: public CppUnit::TestFixture {
private:
	utils::CryptoHash hash;
public:
	/*=== BEGIN tests for class 'DoubleHashingScheme' ===*/
	void testCount();
	void testHash();
	/*=== END   tests for class 'DoubleHashingScheme' ===*/

	/*=== BEGIN tests for class 'ExtendedDoubleHashingScheme' ===*/
	void testExtendedCount();
	void testExtendedHash();
	/*=== END   tests for class 'ExtendedDoubleHashingScheme' ===*/

	void setUp();
	void tearDown();

	CPPUNIT_TEST_SUITE( DoubleHashingSchemeTest);
		CPPUNIT_TEST( testCount);
		CPPUNIT_TEST( testHash);
		CPPUNIT_TEST( testExtendedCount);
		CPPUNIT_TEST( testExtendedHash);
	CPPUNIT_TEST_SUITE_END();
};
CPPUNIT_TEST_SUITE_REGISTRATION(DoubleHashingSchemeTest);
}
}
#endif /* DOUBLEFILTERSCHEMETEST_H */
