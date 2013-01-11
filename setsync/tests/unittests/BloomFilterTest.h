///
/// @brief       CPPUnit-Tests for class BloomFilter
/// @author      Till Lorentzen (lorentze@ibr.cs.tu-bs.de)
/// 

 
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include <setsync/crypto/CryptoHash.h>

#ifndef BLOOMFILTERTEST_HH
#define BLOOMFILTERTEST_HH
namespace setsync {

namespace bloom {

class BloomFilterTest : public CppUnit::TestFixture {
	private:
	utils::CryptoHash hash;
	public:
		/*=== BEGIN tests for class 'BloomFilter' ===*/
		void testLoad();
		void testInsert();
		void testContains();
		void testContainsAll();
		void testOperatorAndAndAssign();
		void testOperatorInclusiveOrAndAssign();
		void testOperatorXorAndAssign();
		/*=== END   tests for class 'BloomFilter' ===*/

		void setUp();
		void tearDown();


		CPPUNIT_TEST_SUITE(BloomFilterTest);
			CPPUNIT_TEST(testLoad);
			CPPUNIT_TEST(testInsert);
			CPPUNIT_TEST(testContains);
			CPPUNIT_TEST(testContainsAll);
			CPPUNIT_TEST(testOperatorAndAndAssign);
			CPPUNIT_TEST(testOperatorInclusiveOrAndAssign);
			CPPUNIT_TEST(testOperatorXorAndAssign);
		CPPUNIT_TEST_SUITE_END();
};
CPPUNIT_TEST_SUITE_REGISTRATION( BloomFilterTest);
}
}
#endif /* BLOOMFILTERTEST_HH */
