///
/// @brief       CPPUnit-Tests for class BloomFilter
/// @author      Till Lorentzen (lorentze@ibr.cs.tu-bs.de)
/// 

 
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#ifndef BLOOMFILTERTEST_HH
#define BLOOMFILTERTEST_HH
namespace bloom {
class BloomFilterTest : public CppUnit::TestFixture {
	private:
	public:
		/*=== BEGIN tests for class 'DefaultHashProvider' ===*/
		void testCount();
		void testHash();
		/*=== END   tests for class 'DefaultHashProvider' ===*/

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
			CPPUNIT_TEST(testCount);
			CPPUNIT_TEST(testHash);
			CPPUNIT_TEST(testLoad);
			CPPUNIT_TEST(testInsert);
			CPPUNIT_TEST(testContains);
			CPPUNIT_TEST(testContainsAll);
			CPPUNIT_TEST(testOperatorAndAndAssign);
			CPPUNIT_TEST(testOperatorInclusiveOrAndAssign);
			CPPUNIT_TEST(testOperatorXorAndAssign);
		CPPUNIT_TEST_SUITE_END();
};
};
#endif /* BLOOMFILTERTEST_HH */
