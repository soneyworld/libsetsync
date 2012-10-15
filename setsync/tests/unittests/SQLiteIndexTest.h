/*
 * SQLiteIndexTest.h
 *
 *      Author: Till Lorentzen
 */

#ifndef SQLITEINDEXTEST_H_
#define SQLITEINDEXTEST_H_
#include <cppunit/extensions/HelperMacros.h>
#include "setsync/index/SQLiteIndex.h"
namespace sqlite{
class SQLiteIndexTest: public CPPUNIT_NS::TestCase {
	CPPUNIT_TEST_SUITE( SQLiteIndexTest);
	CPPUNIT_TEST( testAddingAndErasingElements);
	CPPUNIT_TEST( testAddingAndCleaningElements);
	CPPUNIT_TEST( testAddingAndFindingElements);
	CPPUNIT_TEST_SUITE_END();
private:
	sqlite::SQLiteDatabase * db;
	sqlite::SQLiteIndex * index;

public:
	void setUp(void);
	void tearDown(void);
protected:
	void testAddingAndErasingElements();
	void testAddingAndCleaningElements();
	void testAddingAndFindingElements();
};
//CPPUNIT_TEST_SUITE_REGISTRATION( SQLiteIndexTest);
};
#endif /* SQLITEINDEXTEST_H_ */
