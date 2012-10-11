///
/// @brief       CPPUnit-Tests for class DBBloomFilter
/// @author      Till Lorentzen (lorentze@ibr.cs.tu-bs.de)
/// 


#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include <setsync/bloom/DBBloomFilter.h>

#ifndef DBBLOOMFILTERTEST_H
#define DBBLOOMFILTERTEST_H
class DBBloomFilterTest: public CppUnit::TestFixture {
private:
	Db * db1;
	Db * db2;
public:
	/*=== BEGIN tests for class 'DBBloomFilter' ===*/
	void testLoad();
	void testInsert();
	void testContains();
	void testContainsAll();
	void testOperatorAndAndAssign();
	void testOperatorInclusiveOrAndAssign();
	void testOperatorXorAndAssign();
	/*=== END   tests for class 'DBBloomFilter' ===*/

	void setUp();
	void tearDown();

CPPUNIT_TEST_SUITE(DBBloomFilterTest);
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
