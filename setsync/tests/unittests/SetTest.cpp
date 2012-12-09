/*
 * SetTest.cpp
 *
 *      Author: Till Lorentzen
 */

#include "SetTest.h"
namespace setsync {
void SetTest::testInsert() {
	setsync::Set set(config);
	CPPUNIT_ASSERT(set.getSize() == 0);
	CPPUNIT_ASSERT(set.insert("hallo"));
	CPPUNIT_ASSERT(set.getSize() == 1);
}
void SetTest::testErase() {
	setsync::Set set(config);
	CPPUNIT_ASSERT(set.getSize() == 0);
	CPPUNIT_ASSERT(!set.erase("hallo"));
	CPPUNIT_ASSERT(set.getSize() == 0);
	CPPUNIT_ASSERT(set.insert("hallo"));
	CPPUNIT_ASSERT(set.getSize() == 1);
	CPPUNIT_ASSERT(set.erase("hallo"));
	CPPUNIT_ASSERT(set.getSize() == 0);
	CPPUNIT_ASSERT(!set.erase("hallo"));
}
void SetTest::testClear() {
	setsync::Set set(config);
	CPPUNIT_ASSERT(set.getSize() == 0);
	CPPUNIT_ASSERT(set.insert("hallo"));
	CPPUNIT_ASSERT(set.getSize() == 1);
	set.clear();
	CPPUNIT_ASSERT(set.getSize() == 0);
}

void SetTest::testFind() {
	setsync::Set set(config);
	CPPUNIT_ASSERT(set.getSize() == 0);
	CPPUNIT_ASSERT(!set.find("hallo"));
	CPPUNIT_ASSERT(set.insert("hallo"));
	CPPUNIT_ASSERT(set.getSize() == 1);
	CPPUNIT_ASSERT(set.find("hallo"));
	CPPUNIT_ASSERT(set.erase("hallo"));
	CPPUNIT_ASSERT(set.getSize() == 0);
	CPPUNIT_ASSERT(!set.find("hallo"));
}

void SetTest::testMaximum() {
	config::Configuration::BloomFilterConfig bfconfig;
	bfconfig.setHardMaximum(true);
	bfconfig.setMaxElements(5);
	config::Configuration maxConfig(bfconfig);
	maxConfig.setPath(dir->getPath());
	setsync::Set set(maxConfig);
	CPPUNIT_ASSERT(set.getSize() == 0);
	CPPUNIT_ASSERT(set.insert("1"));
	CPPUNIT_ASSERT(set.getSize() == 1);
	CPPUNIT_ASSERT(set.insert("2"));
	CPPUNIT_ASSERT(set.getSize() == 2);
	CPPUNIT_ASSERT(set.insert("3"));
	CPPUNIT_ASSERT(set.getSize() == 3);
	CPPUNIT_ASSERT(set.insert("4"));
	CPPUNIT_ASSERT(set.getSize() == 4);
	CPPUNIT_ASSERT(set.insert("5"));
	CPPUNIT_ASSERT(set.getSize() == 5);
	CPPUNIT_ASSERT(!set.insert("6"));
	CPPUNIT_ASSERT(set.getSize() == 5);
	CPPUNIT_ASSERT(set.erase("5"));
	CPPUNIT_ASSERT(set.getSize() == 4);
	CPPUNIT_ASSERT(set.insert("6"));
	CPPUNIT_ASSERT(set.getSize() == 5);
}

void SetTest::setUp() {
	this->dir = new utils::FileSystem::TemporaryDirectory("temp_");
	this->config.setPath(dir->getPath());
}
void SetTest::tearDown() {
	delete this->dir;
}
}
;
