///
/// @file        DBBloomFilterTest.cpp
/// @brief       CPPUnit-Tests for class DBBloomFilter
/// @author      Till Lorentzen (lorentze@ibr.cs.tu-bs.de)
/// 

#include "BdbBloomFilterTest.h"
#include <setsync/bloom/BdbBloomFilter.h>
#include <setsync/utils/FileSystem.h>
#include <iostream>
#include <vector>
#include <string>
#include <string.h>
#include <math.h>
#include <list>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>

using namespace std;
namespace bloom {

/*=== BEGIN tests for class 'DBBloomFilter' ===*/
void BdbBloomFilterTest::testConstructor() {
	bloom::BdbBloomFilter Filter1(hashFunction_, db1, 10, false, 0.01);
	Filter1.AbstractBloomFilter::add("bla1");
	Filter1.AbstractBloomFilter::add("bla2");
	Filter1.AbstractBloomFilter::add("bla3");
	Filter1.AbstractBloomFilter::add("bla4");
	bloom::BdbBloomFilter Filter2(hashFunction_, db1, 10, false, 0.01);
	CPPUNIT_ASSERT(Filter1.AbstractBloomFilter::contains("bla1"));
	CPPUNIT_ASSERT(Filter2.AbstractBloomFilter::contains("bla1"));
	CPPUNIT_ASSERT(Filter2.AbstractBloomFilter::contains("bla2"));
	CPPUNIT_ASSERT(Filter2.AbstractBloomFilter::contains("bla3"));
	CPPUNIT_ASSERT(Filter2.AbstractBloomFilter::contains("bla4"));
}

void BdbBloomFilterTest::testLoad() {
	bloom::BdbBloomFilter Filter1(hashFunction_, db1, 10, false, 0.01);
	bloom::BdbBloomFilter Filter2(hashFunction_, db2, 10, false, 0.01);
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

void BdbBloomFilterTest::testInsert() {
	DB_BTREE_STAT *dbstat;
	unsigned int nrecords;
	db1->stat(NULL, &dbstat, 0);
	nrecords = dbstat->bt_ndata;
	free(dbstat);
	CPPUNIT_ASSERT(nrecords == 0);
	bloom::BdbBloomFilter Filter1(hashFunction_, db1, 10, false, 0.01);
	//	 test signature (const unsigned char* key)
	unsigned char cad1[hashFunction_.getHashSize()];
	hashFunction_(cad1, "ejemplo");
	Filter1.add(cad1);
	db1->stat(NULL, &dbstat, 0);
	nrecords = dbstat->bt_ndata;
	free(dbstat);
	CPPUNIT_ASSERT(nrecords == Filter1.functionCount_);
	CPPUNIT_ASSERT_EQUAL(true, Filter1.contains(cad1));

	//	 test signature (const std::string& key)
	Filter1.AbstractBloomFilter::add(string("hello"));
	CPPUNIT_ASSERT_EQUAL(true, Filter1.AbstractBloomFilter::contains(string("hello")));
	db1->stat(NULL, &dbstat, 0);
	nrecords = dbstat->bt_ndata;
	free(dbstat);
	CPPUNIT_ASSERT(nrecords == Filter1.functionCount_*2);
	//	 test signature (const char* key)

	Filter1.AbstractBloomFilter::add("bye");
	CPPUNIT_ASSERT_EQUAL(true, Filter1.AbstractBloomFilter::contains("bye"));
	db1->stat(NULL, &dbstat, 0);
	nrecords = dbstat->bt_ndata;
	free(dbstat);
	CPPUNIT_ASSERT(nrecords == Filter1.functionCount_*3);

	/// Testing transaction code
	bloom::BdbBloomFilter Filter3(hashFunction_, db3, 10, false, 0.01);
	Filter3.AbstractBloomFilter::add("bla1");
	Filter3.AbstractBloomFilter::add("bla2");
	Filter3.AbstractBloomFilter::add("bla3");
	CPPUNIT_ASSERT(Filter3.itemCount_== 3);
}

void BdbBloomFilterTest::testRemove() {
	//cout << endl;
	DB_BTREE_STAT *dbstat;
	unsigned int nrecords;
	bloom::BdbBloomFilter Filter1(hashFunction_, db1, 10, false, 0.01);
	bloom::BdbBloomFilter Filter2(hashFunction_, db2, 10, false, 0.01);
	bloom::BdbBloomFilter Filter3(hashFunction_, db3, 10, false, 0.01);
	//	 test signature (const unsigned char* key)

	unsigned char cad1[hashFunction_.getHashSize()];
	hashFunction_(cad1, "ejemplo");
	CPPUNIT_ASSERT(!Filter1.remove(cad1));
	Filter1.add(cad1);
	db1->stat(NULL, &dbstat, 0);
	nrecords = dbstat->bt_ndata;
	free(dbstat);
	CPPUNIT_ASSERT(nrecords == Filter1.functionCount_);
	CPPUNIT_ASSERT_EQUAL(true, Filter1.contains(cad1));
	bool removed = Filter1.remove(cad1);
	CPPUNIT_ASSERT(removed);
	db1->stat(NULL, &dbstat, 0);
	nrecords = dbstat->bt_ndata;
	free(dbstat);
	CPPUNIT_ASSERT(nrecords == 0);
	CPPUNIT_ASSERT_EQUAL(false, Filter1.contains(cad1));
	CPPUNIT_ASSERT(!Filter1.remove(cad1));
	//	 test signature (const std::string& key)
	Filter1.AbstractBloomFilter::add(string("hello"));
	Filter1.AbstractBloomFilter::add(string("hello"));
	db1->stat(NULL, &dbstat, 0);
	nrecords = dbstat->bt_ndata;
	free(dbstat);
	CPPUNIT_ASSERT(nrecords == Filter1.functionCount_);
	CPPUNIT_ASSERT_EQUAL(true, Filter1.AbstractBloomFilter::contains(string("hello")));
	CPPUNIT_ASSERT(Filter1.CountingBloomFilter::remove(string("hello")));
	db1->stat(NULL, &dbstat, 0);
	nrecords = dbstat->bt_ndata;
	free(dbstat);
	CPPUNIT_ASSERT(nrecords == 0);
	CPPUNIT_ASSERT(!Filter1.AbstractBloomFilter::contains(string("hello")));
	//	 test signature (const char* key)
	CPPUNIT_ASSERT(!Filter1.CountingBloomFilter::remove("bye"));
	db1->stat(NULL, &dbstat, 0);
	nrecords = dbstat->bt_ndata;
	free(dbstat);
	CPPUNIT_ASSERT(nrecords == 0);
	Filter1.AbstractBloomFilter::add("bye");
	db1->stat(NULL, &dbstat, 0);
	nrecords = dbstat->bt_ndata;
	free(dbstat);
	CPPUNIT_ASSERT(nrecords == Filter1.functionCount_);
	CPPUNIT_ASSERT(Filter1.AbstractBloomFilter::contains("bye"));
	CPPUNIT_ASSERT(Filter1.CountingBloomFilter::remove("bye"));
	db1->stat(NULL, &dbstat, 0);
	nrecords = dbstat->bt_ndata;
	free(dbstat);
	CPPUNIT_ASSERT(nrecords == 0);
	CPPUNIT_ASSERT(!Filter1.CountingBloomFilter::remove("bye"));
	Filter1.AbstractBloomFilter::add("bla1");
	Filter2.AbstractBloomFilter::add("bla1");
	//std::cout << Filter2.toString() << " bla1"<< std::endl;
	Filter2.clear();
	db1->stat(NULL, &dbstat, 0);
	nrecords = dbstat->bt_ndata;
	//cout << Filter1.toString() << " bla1"<< endl;
	free(dbstat);
	CPPUNIT_ASSERT(nrecords == Filter1.functionCount_);
	Filter1.AbstractBloomFilter::add("bla2");
	Filter2.AbstractBloomFilter::add("bla2");
	//std::cout << Filter2.toString() << " bla2"<< std::endl;
	Filter2.clear();
	db1->stat(NULL, &dbstat, 0);
	nrecords = dbstat->bt_ndata;
	//cout << Filter1.toString() << " bla1+bla2"<< endl;
	free(dbstat);
	CPPUNIT_ASSERT(nrecords == Filter1.functionCount_*2);
	Filter1.AbstractBloomFilter::add("bla3");
	Filter2.AbstractBloomFilter::add("bla3");
	//std::cout << Filter2.toString() << " bla3"<< std::endl;
	Filter2.clear();
	db1->stat(NULL, &dbstat, 0);
	nrecords = dbstat->bt_ndata;
	free(dbstat);
	//cout << Filter1.toString() << " bla1+bla2+bla3"<< endl;
	CPPUNIT_ASSERT(nrecords == Filter1.functionCount_*3);
	CPPUNIT_ASSERT(Filter1.CountingBloomFilter::remove("bla1"));
	db1->stat(NULL, &dbstat, 0);
	nrecords = dbstat->bt_ndata;
	free(dbstat);
	//	cout << Filter1.toString() << " bla2+bla3"<< endl;
	CPPUNIT_ASSERT(nrecords == Filter1.functionCount_*2);
	CPPUNIT_ASSERT(Filter1.CountingBloomFilter::remove("bla2"));
	db1->stat(NULL, &dbstat, 0);
	nrecords = dbstat->bt_ndata;
	free(dbstat);
	//	cout << Filter1.toString() << " bla3"<< endl;
	CPPUNIT_ASSERT(nrecords == Filter1.functionCount_);
	CPPUNIT_ASSERT(Filter1.AbstractBloomFilter::contains("bla3"));
	CPPUNIT_ASSERT(Filter1.CountingBloomFilter::remove("bla3"));
	db1->stat(NULL, &dbstat, 0);
	nrecords = dbstat->bt_ndata;
	free(dbstat);
	CPPUNIT_ASSERT(nrecords == 0);

	/// Testing transaction code
	Filter3.AbstractBloomFilter::add("bla1");
	Filter3.AbstractBloomFilter::add("bla2");
	Filter3.AbstractBloomFilter::add("bla3");
	CPPUNIT_ASSERT(Filter3.CountingBloomFilter::remove("bla1"));
	CPPUNIT_ASSERT(Filter3.CountingBloomFilter::remove("bla2"));
	CPPUNIT_ASSERT(Filter3.CountingBloomFilter::remove("bla3"));

}

void BdbBloomFilterTest::testContains() {
	/* test signature (const std::string& key) const */
	/* test signature (const char* data, const std::size_t& length) const */
	bloom::BdbBloomFilter Filter1(hashFunction_, db1, 8196);
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

void BdbBloomFilterTest::testContainsAll() {
	bloom::BdbBloomFilter Filter2(hashFunction_, db1, 8196);
	unsigned char hashes[100 * hashFunction_.getHashSize()];
	for (int j = 0; j < 100; j++) {
		unsigned char word[8];
		for (int i = 0; i <= 7; i++) {
			word[i] = 33 + rand() % (126 - 23);
		}
		hashFunction_(hashes + hashFunction_.getHashSize() * j, word, 8);
	}
	for (int i = 0; i < 100; i++) {
		Filter2.add(hashes + i * hashFunction_.getHashSize());
	}
	Filter2.containsAll(hashes, 100);
	CPPUNIT_ASSERT(Filter2.containsAll(hashes, 100));
}

void BdbBloomFilterTest::testOperatorAndAndAssign() {
	/* test signature (const BloomFilter& filter) */
	bloom::BdbBloomFilter FilterA(hashFunction_, db1, 8196);
	bloom::BdbBloomFilter FilterB(hashFunction_, db2, 8196);

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

void BdbBloomFilterTest::testOperatorInclusiveOrAndAssign() {
	/* test signature (const BloomFilter& filter) */
	bloom::BdbBloomFilter FilterA(hashFunction_, db1, 8196);
	bloom::BdbBloomFilter FilterB(hashFunction_, db2, 8196);

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

void BdbBloomFilterTest::testOperatorXorAndAssign() {
	/* test signature (const BloomFilter& filter) */
	bloom::BdbBloomFilter FilterA(hashFunction_, db1, 8196);
	bloom::BdbBloomFilter FilterB(hashFunction_, db2, 8196);

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

void BdbBloomFilterTest::testSavingAndLoadingSettings() {
	CPPUNIT_ASSERT_THROW(bloom::BdbBloomFilter::loadSettings(this->db1),DbException);
	uint64_t maxElements = 10000;
	bool hardMax = false;
	float rate = 0.001;
	std::size_t hashsize = 20;
	bloom::BdbBloomFilter::saveSettings(this->db1, maxElements, hardMax, rate,
			hashsize);
	bloom::BdbBloomFilterSetting loaded = bloom::BdbBloomFilter::loadSettings(
			this->db1);
	CPPUNIT_ASSERT_EQUAL(loaded.maxNumberOfElements, maxElements);
	CPPUNIT_ASSERT_EQUAL(loaded.hardMaximum,hardMax);
	CPPUNIT_ASSERT_EQUAL(loaded.falsePositiveRate,rate);
	CPPUNIT_ASSERT_EQUAL(loaded.hashSize, hashsize);

	maxElements = 100;
	hardMax = true;
	rate = 0.01;
	hashsize = 40;
	bloom::BdbBloomFilter::saveSettings(this->db1, maxElements, hardMax, rate,
			hashsize);
	loaded = bloom::BdbBloomFilter::loadSettings(this->db1);
	CPPUNIT_ASSERT_EQUAL(loaded.maxNumberOfElements, maxElements);
	CPPUNIT_ASSERT_EQUAL(loaded.hardMaximum,hardMax);
	CPPUNIT_ASSERT_EQUAL(loaded.falsePositiveRate,rate);
	CPPUNIT_ASSERT_EQUAL(loaded.hashSize, hashsize);
}

void BdbBloomFilterTest::testDiff() {
	bloom::BdbBloomFilter FilterA(hashFunction_, db1, 8196);
	bloom::BdbBloomFilter FilterB(hashFunction_, db2, 8196);
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
	unsigned char sha[hashFunction_.getHashSize()];
	hashFunction_(sha, "bla3");
	CPPUNIT_ASSERT(memcmp(handler[0].first,sha,hashFunction_.getHashSize())==0);
	hashFunction_(sha, "bla4");
	CPPUNIT_ASSERT(memcmp(handler[1].first,sha,hashFunction_.getHashSize())==0);
}

void BdbBloomFilterTest::testToString() {
	//cout << endl;
	bloom::BdbBloomFilter FilterA(hashFunction_, db1, 10);
	bloom::BdbBloomFilter FilterB(hashFunction_, db2, 10);
	//cout << FilterA.toString() << endl;
	for (unsigned int i = 1; i <= 10; i++) {
		stringstream ss;
		ss << "test" << i;
		FilterA.AbstractBloomFilter::add(ss.str());
		FilterB.AbstractBloomFilter::add(ss.str());
		FilterB.toString();
		//cout << FilterB.toString() << " " << ss.str() << endl;
		FilterB.clear();
	}
	FilterA.toString();
	//cout << FilterA.toString() << endl;
}

/*=== END   tests for class 'DBBloomFilter' ===*/

void BdbBloomFilterTest::setUp() {
	this->db1 = new Db(NULL, 0);
	this->db2 = new Db(NULL, 0);
	db1->set_flags(bloom::BdbBloomFilter::getTableFlags());
	db1->open(NULL, "table1.db",
			bloom::BdbBloomFilter::getLogicalDatabaseName(),
			bloom::BdbBloomFilter::getTableType(), DB_CREATE, 0);
	db2->set_flags(bloom::BdbBloomFilter::getTableFlags());
	db2->open(NULL, "table2.db",
			bloom::BdbBloomFilter::getLogicalDatabaseName(),
			bloom::BdbBloomFilter::getTableType(), DB_CREATE, 0);
	int ret = 0;
	this->env1 = new DbEnv(0);
	mkdir("table3", 0700);
	try {
		ret = this->env1->open("table3", DB_INIT_MPOOL | DB_CREATE, 0);
	} catch (DbException e) {
		cout << endl << ret << endl;
		cout << e.what() << endl;
		cout << e.get_errno() << endl;
	}
	this->db3 = new Db(this->env1, 0);
	ret = db3->set_flags(bloom::BdbBloomFilter::getTableFlags());
	ret = db3->open(NULL, "bf", bloom::BdbBloomFilter::getLogicalDatabaseName(),
			bloom::BdbBloomFilter::getTableType(), DB_CREATE, 0);
}

void BdbBloomFilterTest::tearDown() {
	try {
		this->db1->close(0);
		this->db2->close(0);
		delete this->db1;
		delete this->db2;
		this->db1 = new Db(NULL, 0);
		this->db2 = new Db(NULL, 0);
		db1->remove("table1.db", NULL, 0);
		db2->remove("table2.db", NULL, 0);
		this->db3->close(0);
		this->env1->close(0);
		utils::FileSystem::rmDirRecursive("table3");
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
	delete this->db3;
	delete this->env1;
}
}
;
