///
/// @brief       CPPUnit-Tests for class BloomFilter
/// @author      Till Lorentzen (lorentze@ibr.cs.tu-bs.de)
/// 


#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include <setsync/crypto/CryptoHash.h>

#ifndef FSBLOOMFILTERTEST_H
#define FSBLOOMFILTERTEST_H
namespace setsync {
namespace bloom {
class FSBloomFilterTest: public CppUnit::TestFixture {
private:
	crypto::CryptoHash hash;
public:
	/*=== BEGIN tests for class 'FSBloomFilter' ===*/
	void testLoad();
	void testInsert();
	void testContains();
	void testContainsAll();
	void testOperatorAndAndAssign();
	void testOperatorInclusiveOrAndAssign();
	void testOperatorXorAndAssign();
	void testFilterSize();
	/*=== END   tests for class 'FSBloomFilter' ===*/

	void setUp();
	void tearDown();

CPPUNIT_TEST_SUITE(FSBloomFilterTest);
		CPPUNIT_TEST(testLoad);
		CPPUNIT_TEST(testInsert);
		CPPUNIT_TEST(testContains);
		CPPUNIT_TEST(testContainsAll);
		CPPUNIT_TEST(testOperatorAndAndAssign);
		CPPUNIT_TEST(testOperatorInclusiveOrAndAssign);
		CPPUNIT_TEST(testOperatorXorAndAssign);
		CPPUNIT_TEST(testFilterSize);
	CPPUNIT_TEST_SUITE_END();
};
CPPUNIT_TEST_SUITE_REGISTRATION( FSBloomFilterTest);
}
}
#endif /* BLOOMFILTERTEST_HH */
