///
/// @file        DBBloomFilterTest.cpp
/// @brief       CPPUnit-Tests for class DBBloomFilter
/// @author      Till Lorentzen (lorentze@ibr.cs.tu-bs.de)
/// 

#include "DBBloomFilterTest.h"
#include <setsync/bloom/DBBloomFilter.h>
#include <iostream>
#include <vector>
#include <string>
#include <string.h>
#include <math.h>
#include <list>
#include <setsync/sha1.h>
#include <stdlib.h>

using namespace std;
namespace bloom {

/*=== BEGIN tests for class 'DBBloomFilter' ===*/
void DBBloomFilterTest::testConstructor() {
	bloom::DBBloomFilter Filter1(db1, 10, false, 0.01);
	Filter1.AbstractBloomFilter::add("bla1");
	Filter1.AbstractBloomFilter::add("bla2");
	Filter1.AbstractBloomFilter::add("bla3");
	Filter1.AbstractBloomFilter::add("bla4");
	bloom::DBBloomFilter Filter2(db1, 10, false, 0.01);
	CPPUNIT_ASSERT(Filter1.AbstractBloomFilter::contains("bla1"));
	CPPUNIT_ASSERT(Filter2.AbstractBloomFilter::contains("bla1"));
	CPPUNIT_ASSERT(Filter2.AbstractBloomFilter::contains("bla2"));
	CPPUNIT_ASSERT(Filter2.AbstractBloomFilter::contains("bla3"));
	CPPUNIT_ASSERT(Filter2.AbstractBloomFilter::contains("bla4"));
}

void DBBloomFilterTest::testLoad() {
	bloom::DBBloomFilter Filter1(db1, 10, false, 0.01);
	bloom::DBBloomFilter Filter2(db2, 10, false, 0.01);
	Filter1.AbstractBloomFilter::add("hello");
	CPPUNIT_ASSERT_EQUAL(true, Filter1.AbstractBloomFilter::contains("hello"));
	CPPUNIT_ASSERT_EQUAL(false, Filter2.AbstractBloomFilter::contains("hello"));
	std::stringstream buf;
	Filter1.save(buf);
	Filter2.load(buf, Filter1.numberOfElements());
	CPPUNIT_ASSERT_EQUAL(Filter1.numberOfElements(), Filter2.numberOfElements());
	//	std::cout << std::endl << Filter1.toString() << std::endl;
	//	std::cout << std::endl << Filter2.toString() << std::endl;
	CPPUNIT_ASSERT(Filter1 == Filter2);
	CPPUNIT_ASSERT(Filter2.AbstractBloomFilter::contains("hello"));
}

void DBBloomFilterTest::testInsert() {
	bloom::DBBloomFilter Filter1(db1, 10, false, 0.01);

	//	 test signature (const unsigned char* key)

	unsigned char cad1[SHA_DIGEST_LENGTH];
	SHA1((const unsigned char*) "ejemplo", 7, cad1);
	Filter1.add(cad1);
	CPPUNIT_ASSERT_EQUAL(true, Filter1.contains(cad1));

	//	 test signature (const std::string& key)
	Filter1.AbstractBloomFilter::add(string("hello"));
	CPPUNIT_ASSERT_EQUAL(true, Filter1.AbstractBloomFilter::contains(string("hello")));

	//	 test signature (const char* key)

	Filter1.AbstractBloomFilter::add("bye");
	CPPUNIT_ASSERT_EQUAL(true, Filter1.AbstractBloomFilter::contains("bye"));
	//std::cout << std::endl << Filter1.toString() << std::endl;

}

void DBBloomFilterTest::testRemove() {
	bloom::DBBloomFilter Filter1(db1, 10, false, 0.01);

	//	 test signature (const unsigned char* key)

	unsigned char cad1[SHA_DIGEST_LENGTH];
	SHA1((const unsigned char*) "ejemplo", 7, cad1);
	CPPUNIT_ASSERT(!Filter1.remove(cad1));
	Filter1.add(cad1);
	CPPUNIT_ASSERT_EQUAL(true, Filter1.contains(cad1));
	bool removed = Filter1.remove(cad1);
	CPPUNIT_ASSERT(removed);
	CPPUNIT_ASSERT_EQUAL(false, Filter1.contains(cad1));
	CPPUNIT_ASSERT(!Filter1.remove(cad1));
	//	 test signature (const std::string& key)
	Filter1.AbstractBloomFilter::add(string("hello"));
	Filter1.AbstractBloomFilter::add(string("hello"));
	CPPUNIT_ASSERT_EQUAL(true, Filter1.AbstractBloomFilter::contains(string("hello")));
	CPPUNIT_ASSERT(Filter1.CountingBloomFilter::remove(string("hello")));
	CPPUNIT_ASSERT(!Filter1.AbstractBloomFilter::contains(string("hello")));
	//	 test signature (const char* key)
	CPPUNIT_ASSERT(!Filter1.CountingBloomFilter::remove("bye"));
	Filter1.AbstractBloomFilter::add("bye");
	CPPUNIT_ASSERT(Filter1.AbstractBloomFilter::contains("bye"));
	CPPUNIT_ASSERT(Filter1.CountingBloomFilter::remove("bye"));
	CPPUNIT_ASSERT(!Filter1.CountingBloomFilter::remove("bye"));
	//std::cout << std::endl << Filter1.toString() << std::endl;
	Filter1.AbstractBloomFilter::add("bla1");
	Filter1.AbstractBloomFilter::add("bla2");
	Filter1.AbstractBloomFilter::add("bla3");
	CPPUNIT_ASSERT(Filter1.CountingBloomFilter::remove("bla1"));
	CPPUNIT_ASSERT(Filter1.CountingBloomFilter::remove("bla2"));
	CPPUNIT_ASSERT(Filter1.CountingBloomFilter::remove("bla3"));
	//std::cout << std::endl << Filter1.toString() << std::endl;
}

void DBBloomFilterTest::testContains() {
	/* test signature (const std::string& key) const */
	/* test signature (const char* data, const std::size_t& length) const */
	bloom::DBBloomFilter Filter1(db1, 8196);
	char word[8];
	for (int j = 0; j <= 127; j++) {
		for (int i = 0; i <= 7; i++) {
			word[i] = 33 + rand() % (126 - 23);
		}
		Filter1.AbstractBloomFilter::add(word);
		CPPUNIT_ASSERT_EQUAL(true, Filter1.AbstractBloomFilter::contains(word));
	}
	Filter1.clear();
	CPPUNIT_ASSERT_EQUAL(false, Filter1.AbstractBloomFilter::contains(word));

}

void DBBloomFilterTest::testContainsAll() {
	bloom::DBBloomFilter Filter2(db1, 8196);
	unsigned char hashes[100 * SHA_DIGEST_LENGTH];
	for (int j = 0; j < 100; j++) {
		unsigned char word[8];
		for (int i = 0; i <= 7; i++) {
			word[i] = 33 + rand() % (126 - 23);
		}
		SHA1(word, 8, hashes + SHA_DIGEST_LENGTH * j);
	}
	for (int i = 0; i < 100; i++) {
		Filter2.add(hashes + i * SHA_DIGEST_LENGTH);
	}
	Filter2.containsAll(hashes, 100);
	CPPUNIT_ASSERT(Filter2.containsAll(hashes, 100));
}

void DBBloomFilterTest::testOperatorAndAndAssign() {
	/* test signature (const BloomFilter& filter) */
	bloom::DBBloomFilter FilterA(db1, 8196);
	bloom::DBBloomFilter FilterB(db2, 8196);

	string strin1, strin2, strin3, strin4;
	strin1 = "hello";
	strin2 = "byebye";
	FilterA.AbstractBloomFilter::add(strin1);
	FilterA.AbstractBloomFilter::add(strin2);
	CPPUNIT_ASSERT_EQUAL(true, FilterA.AbstractBloomFilter::contains(strin1));
	CPPUNIT_ASSERT_EQUAL(true, FilterA.AbstractBloomFilter::contains(strin2));

	strin3 = "hola";
	strin4 = "byebye";
	FilterB.AbstractBloomFilter::add(strin3);
	FilterB.AbstractBloomFilter::add(strin4);
	CPPUNIT_ASSERT_EQUAL(true, FilterB.AbstractBloomFilter::contains(strin3));
	CPPUNIT_ASSERT_EQUAL(true, FilterB.AbstractBloomFilter::contains(strin4));

	FilterA.operator &=(FilterB);
	CPPUNIT_ASSERT_EQUAL(false, FilterA.AbstractBloomFilter::contains(strin1));
	CPPUNIT_ASSERT_EQUAL(true, FilterA.AbstractBloomFilter::contains(strin2));
	CPPUNIT_ASSERT_EQUAL(false, FilterA.AbstractBloomFilter::contains(strin3));
	CPPUNIT_ASSERT_EQUAL(true, FilterA.AbstractBloomFilter::contains(strin4));

}

void DBBloomFilterTest::testOperatorInclusiveOrAndAssign() {
	/* test signature (const BloomFilter& filter) */
	bloom::DBBloomFilter FilterA(db1, 8196);
	bloom::DBBloomFilter FilterB(db2, 8196);

	string strin1, strin2, strin3, strin4;
	strin1 = "hello";
	strin2 = "byebye";
	FilterA.AbstractBloomFilter::add(strin1);
	FilterA.AbstractBloomFilter::add(strin2);
	CPPUNIT_ASSERT_EQUAL(true, FilterA.AbstractBloomFilter::contains(strin1));
	CPPUNIT_ASSERT_EQUAL(true, FilterA.AbstractBloomFilter::contains(strin2));

	strin3 = "hola";
	strin4 = "chau";
	FilterB.AbstractBloomFilter::add(strin3);
	FilterB.AbstractBloomFilter::add(strin4);
	CPPUNIT_ASSERT_EQUAL(true, FilterB.AbstractBloomFilter::contains(strin3));
	CPPUNIT_ASSERT_EQUAL(true, FilterB.AbstractBloomFilter::contains(strin4));

	FilterA.operator |=(FilterB);
	CPPUNIT_ASSERT_EQUAL(true, FilterA.AbstractBloomFilter::contains(strin1));
	CPPUNIT_ASSERT_EQUAL(true, FilterA.AbstractBloomFilter::contains(strin2));
	CPPUNIT_ASSERT_EQUAL(true, FilterA.AbstractBloomFilter::contains(strin3));
	CPPUNIT_ASSERT_EQUAL(true, FilterA.AbstractBloomFilter::contains(strin4));

}

void DBBloomFilterTest::testOperatorXorAndAssign() {
	/* test signature (const BloomFilter& filter) */
	bloom::DBBloomFilter FilterA(db1, 8196);
	bloom::DBBloomFilter FilterB(db2, 8196);

	string strin1, strin2, strin3, strin4;
	strin1 = "hello";
	strin2 = "byebye";
	FilterA.AbstractBloomFilter::add(strin1);
	FilterA.AbstractBloomFilter::add(strin2);
	CPPUNIT_ASSERT_EQUAL(true, FilterA.AbstractBloomFilter::contains(strin1));
	CPPUNIT_ASSERT_EQUAL(true, FilterA.AbstractBloomFilter::contains(strin2));

	strin3 = "hola";
	strin4 = "byebye";
	FilterB.AbstractBloomFilter::add(strin3);
	FilterB.AbstractBloomFilter::add(strin4);
	CPPUNIT_ASSERT_EQUAL(true, FilterB.AbstractBloomFilter::contains(strin3));
	CPPUNIT_ASSERT_EQUAL(true, FilterB.AbstractBloomFilter::contains(strin4));

	FilterA.operator ^=(FilterB);
	CPPUNIT_ASSERT_EQUAL(true, FilterA.AbstractBloomFilter::contains(strin1));
	CPPUNIT_ASSERT_EQUAL(false, FilterA.AbstractBloomFilter::contains(strin2));
	CPPUNIT_ASSERT_EQUAL(true, FilterA.AbstractBloomFilter::contains(strin3));
	CPPUNIT_ASSERT_EQUAL(false, FilterA.AbstractBloomFilter::contains(strin4));

}

void DBBloomFilterTest::testSavingAndLoadingSettings() {
	CPPUNIT_ASSERT_THROW(bloom::DBBloomFilter::loadSettings(this->db1),DbException);
	uint64_t maxElements = 10000;
	bool hardMax = false;
	float rate = 0.001;
	std::size_t hashsize = 20;
	bloom::DBBloomFilter::saveSettings(this->db1, maxElements, hardMax, rate,
			hashsize);
	bloom::DbBloomFilterSetting loaded = bloom::DBBloomFilter::loadSettings(
			this->db1);
	CPPUNIT_ASSERT_EQUAL(loaded.maxNumberOfElements, maxElements);
	CPPUNIT_ASSERT_EQUAL(loaded.hardMaximum,hardMax);
	CPPUNIT_ASSERT_EQUAL(loaded.falsePositiveRate,rate);
	CPPUNIT_ASSERT_EQUAL(loaded.hashSize, hashsize);

	maxElements = 100;
	hardMax = true;
	rate = 0.01;
	hashsize = 40;
	bloom::DBBloomFilter::saveSettings(this->db1, maxElements, hardMax, rate,
			hashsize);
	loaded = bloom::DBBloomFilter::loadSettings(this->db1);
	CPPUNIT_ASSERT_EQUAL(loaded.maxNumberOfElements, maxElements);
	CPPUNIT_ASSERT_EQUAL(loaded.hardMaximum,hardMax);
	CPPUNIT_ASSERT_EQUAL(loaded.falsePositiveRate,rate);
	CPPUNIT_ASSERT_EQUAL(loaded.hashSize, hashsize);
}

void DBBloomFilterTest::testDiff() {
	bloom::DBBloomFilter FilterA(db1, 8196);
	bloom::DBBloomFilter FilterB(db2, 8196);
	FilterA.AbstractBloomFilter::add("bla1");
	FilterA.AbstractBloomFilter::add("bla2");
	FilterA.AbstractBloomFilter::add("bla3");
	FilterB.AbstractBloomFilter::add("bla1");
	FilterB.AbstractBloomFilter::add("bla2");
	unsigned char buffer[1024];
	std::streamsize length = 0;
	std::size_t offset = 0;
	setsync::ListDiffHandler handler;
	std::stringstream buf;
	FilterB.save(buf);
	while (buf.rdbuf()->in_avail() > 0) {
		length = buf.readsome((char*) buffer, 1024);
		FilterA.diff(buffer, length, offset, handler);
		offset += length;
	}
	CPPUNIT_ASSERT(handler.size()==1);
	FilterA.AbstractBloomFilter::add("bla4");
	length = 0;
	offset = 0;
	FilterB.save(buf);
	while (buf.rdbuf()->in_avail() > 0) {
		length = buf.readsome((char*) buffer, 1024);
		FilterA.diff(buffer, length, offset, handler);
		offset += length;
	}
	CPPUNIT_ASSERT(handler.size()==2);
	unsigned char sha[SHA_DIGEST_LENGTH];
	SHA1((unsigned char*)"bla3",strlen("bla3"),sha);
	CPPUNIT_ASSERT(memcmp(handler[0],sha,SHA_DIGEST_LENGTH)==0);
	SHA1((unsigned char*)"bla4",strlen("bla4"),sha);
	CPPUNIT_ASSERT(memcmp(handler[1],sha,SHA_DIGEST_LENGTH)==0);
}

/*=== END   tests for class 'DBBloomFilter' ===*/

void DBBloomFilterTest::setUp() {
	this->db1 = new Db(NULL, 0);
	this->db2 = new Db(NULL, 0);
	db1->set_flags(bloom::DBBloomFilter::getTableFlags());
	db1->open(NULL, "table1.db",
			bloom::DBBloomFilter::getLogicalDatabaseName(),
			bloom::DBBloomFilter::getTableType(), DB_CREATE, 0);
	db2->set_flags(bloom::DBBloomFilter::getTableFlags());
	db2->open(NULL, "table2.db",
			bloom::DBBloomFilter::getLogicalDatabaseName(),
			bloom::DBBloomFilter::getTableType(), DB_CREATE, 0);

}

void DBBloomFilterTest::tearDown() {
	try {
		this->db1->close(0);
		this->db2->close(0);
		delete this->db1;
		delete this->db2;
		this->db1 = new Db(NULL, 0);
		this->db2 = new Db(NULL, 0);
		db1->remove("table1.db", NULL, 0);
		db2->remove("table2.db", NULL, 0);
	}
	// Must catch both DbException and std::exception
	catch (DbException &e) {
		db1->err(e.get_errno(), "Database1 close failed ");
		db2->err(e.get_errno(), "Database2 close failed ");
		throw e;
	} catch (std::exception &e) {
		// No DB error number available, so use errx
		db1->errx("Error closing database: %s", e.what());
		throw e;
	}
	delete this->db1;
	delete this->db2;

}
}
;
