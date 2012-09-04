/*
 * LocalSetTest.cpp
 *
 *      Author: Till Lorentzen
 */

#include "MemSetTest.h"

void MemSetTest::setUp(void) {
	this->memset = new setsync::MemSet();
	this->key_1 = new char[20];

}

void MemSetTest::tearDown(void) {
	delete this->memset;
	delete this->key_1;
}

void MemSetTest::testAddingAndErasingElements() {
	CPPUNIT_ASSERT(memset->getSize() == 0);
	memset->insert(key_1);
	CPPUNIT_ASSERT(memset->getSize() == 1);
	memset->erase(key_1);
	CPPUNIT_ASSERT(memset->getSize() == 0);
	memset->insert(key_1);
	CPPUNIT_ASSERT(memset->getSize() == 1);
}

void MemSetTest::testAddingAndCleaningElements() {
	CPPUNIT_ASSERT(memset->getSize() == 0);
	memset->insert(key_1);
	CPPUNIT_ASSERT(memset->getSize() == 1);
	memset->clear();
	CPPUNIT_ASSERT(memset->getSize() == 0);
}
