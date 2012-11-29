/*
 * BdbSetTest.cpp
 *
 *      Author: Till Lorentzen
 */

#include "BdbSetTest.h"

namespace setsync {
void BdbSetTest::setUp(void) {
	this->key_1 = new unsigned char[20];

}

void BdbSetTest::tearDown(void) {
	delete this->key_1;
}

void BdbSetTest::testAddingAndErasingElements() {
	DbSet set1(hash);
	CPPUNIT_ASSERT(set1.getSize() == 0);
	set1.insert(key_1);
	CPPUNIT_ASSERT(set1.getSize() == 1);
	set1.erase(key_1);
	CPPUNIT_ASSERT(set1.getSize() == 0);
	set1.insert(key_1);
	CPPUNIT_ASSERT(set1.getSize() == 1);
}

void BdbSetTest::testAddingAndCleaningElements() {
	DbSet set1(hash);
	CPPUNIT_ASSERT(set1.getSize() == 0);
	set1.insert(key_1);
	CPPUNIT_ASSERT(set1.getSize() == 1);
	set1.clear();
	CPPUNIT_ASSERT(set1.getSize() == 0);
}
void BdbSetTest::testAdding() {
	DbSet set1(hash, NULL, 10, true, 0.1);
	CPPUNIT_ASSERT(set1.insert("bla1"));
	CPPUNIT_ASSERT(set1.insert("bla2"));
	CPPUNIT_ASSERT(set1.insert("bla3"));
	CPPUNIT_ASSERT(set1.insert("bla4"));
	CPPUNIT_ASSERT(set1.insert("bla5"));
	CPPUNIT_ASSERT(!set1.insert("bla1"));
	CPPUNIT_ASSERT(!set1.insert("bla2"));
	CPPUNIT_ASSERT(!set1.insert("bla3"));
	CPPUNIT_ASSERT(!set1.insert("bla4"));
	CPPUNIT_ASSERT(!set1.insert("bla5"));
}
void BdbSetTest::testSize() {
	DbSet set1(hash,NULL,10, true, 0.1);
	CPPUNIT_ASSERT(set1.getSize() == 0);
	CPPUNIT_ASSERT(set1.insert("bla1"));
	CPPUNIT_ASSERT(set1.getSize() == 1);
	CPPUNIT_ASSERT(!set1.insert("bla1"));
	CPPUNIT_ASSERT(set1.getSize() == 1);
}
void BdbSetTest::testErasing() {
	DbSet set1(hash,NULL,10, true, 0.1);
	CPPUNIT_ASSERT(!set1.erase("bla1"));
	CPPUNIT_ASSERT(set1.insert("bla1"));
	CPPUNIT_ASSERT(set1.erase("bla1"));
	CPPUNIT_ASSERT(!set1.erase("bla1"));
}

void BdbSetTest::testSync(){
	DbSet set1(hash);
	DbSet set2(hash);
	set1.insert("bla1");
	set1.insert("bla2");
	set2.insert("bla1");
	set2.insert("bla2");
//	throw "not yet implemented";
}
}
