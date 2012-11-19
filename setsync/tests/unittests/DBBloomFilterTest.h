///
/// @brief       CPPUnit-Tests for class DBBloomFilter
/// @author      Till Lorentzen (lorentze@ibr.cs.tu-bs.de)
/// 


#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include <setsync/bloom/DBBloomFilter.h>
#include <setsync/utils/CryptoHash.h>

#ifndef DBBLOOMFILTERTEST_H
#define DBBLOOMFILTERTEST_H
namespace bloom {

class DBBloomFilterTest: public CppUnit::TestFixture {
private:
	Db * db1;
	Db * db2;
	DbEnv * env1;
	Db * db3;
	utils::CryptoHash hashFunction_;
public:
	/*=== BEGIN tests for class 'DBBloomFilter' ===*/
	void testConstructor();
	void testLoad();
	void testInsert();
	void testRemove();
	void testContains();
	void testContainsAll();
	void testOperatorAndAndAssign();
	void testOperatorInclusiveOrAndAssign();
	void testOperatorXorAndAssign();
	void testSavingAndLoadingSettings();
	void testDiff();
	void testToString();
	/*=== END   tests for class 'DBBloomFilter' ===*/

	void setUp();
	void tearDown();

CPPUNIT_TEST_SUITE(DBBloomFilterTest);
		CPPUNIT_TEST(testConstructor);
		CPPUNIT_TEST(testLoad);
		CPPUNIT_TEST(testInsert);
		CPPUNIT_TEST(testRemove);
		CPPUNIT_TEST(testContains);
		CPPUNIT_TEST(testContainsAll);
		CPPUNIT_TEST(testOperatorAndAndAssign);
		CPPUNIT_TEST(testOperatorInclusiveOrAndAssign);
		CPPUNIT_TEST(testOperatorXorAndAssign);
		CPPUNIT_TEST(testSavingAndLoadingSettings);
		CPPUNIT_TEST(testDiff);
		CPPUNIT_TEST(testToString);
	CPPUNIT_TEST_SUITE_END();
};
CPPUNIT_TEST_SUITE_REGISTRATION( DBBloomFilterTest);
}
;
#endif /* BLOOMFILTERTEST_HH */
