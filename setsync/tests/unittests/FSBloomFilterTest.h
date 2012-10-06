///
/// @brief       CPPUnit-Tests for class BloomFilter
/// @author      Till Lorentzen (lorentze@ibr.cs.tu-bs.de)
/// 

 
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#ifndef FSBLOOMFILTERTEST_H
#define FSBLOOMFILTERTEST_H
class FSBloomFilterTest : public CppUnit::TestFixture {
	private:
	public:
		/*=== BEGIN tests for class 'FSBloomFilter' ===*/
		void testLoad();
		void testInsert();
		void testContains();
		void testContainsAll();
		void testOperatorAndAndAssign();
		void testOperatorInclusiveOrAndAssign();
		void testOperatorXorAndAssign();
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
		CPPUNIT_TEST_SUITE_END();
};
#endif /* BLOOMFILTERTEST_HH */