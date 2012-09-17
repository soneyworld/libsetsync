/* $Id: templateengine.py 2241 2006-05-22 07:58:58Z fischer $ */

///
/// @file        BloomFilterTest.hh
/// @brief       CPPUnit-Tests for class BloomFilter
/// @author      Author Name (email@mail.address)
/// @date        Created at 2010-11-01
/// 
/// @version     $Revision: 2241 $
/// @note        Last modification: $Date: 2006-05-22 09:58:58 +0200 (Mon, 22 May 2006) $
///              by $Author: fischer $
///

 
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#ifndef BLOOMFILTERTEST_HH
#define BLOOMFILTERTEST_HH
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
#endif /* BLOOMFILTERTEST_HH */
