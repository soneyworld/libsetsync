/*
 * KeyValueCountingBloomFilterTest.cpp
 *
 *      Author: Till Lorentzen
 */

#include "KeyValueCountingBloomFilterTest.h"
#include <setsync/utils/FileSystem.h>
#include <setsync/storage/BdbStorage.h>
#include <setsync/bloom/BdbBloomFilter.h>

using namespace std;
namespace bloom {

void KeyValueCountingBloomFilterTest::setUp() {
	this->db1 = new Db(NULL, 0);
	db1->open(NULL, "table1.db",
			bloom::BdbBloomFilter::getLogicalDatabaseName(),
			bloom::BdbBloomFilter::getTableType(), DB_CREATE, 0);

	path1 = "table1";
	path2 = "table2";
	path3 = "table3";
	this->storage1 = new setsync::storage::LevelDbStorage(path1);
	this->storage2 = new setsync::storage::LevelDbStorage(path2);
	this->storage3 = new setsync::storage::LevelDbStorage(path3);
	this->bdbstorage = new setsync::storage::BdbStorage(this->db1);
}

void KeyValueCountingBloomFilterTest::tearDown() {
	this->db1->close(0);
	delete this->db1;
	this->db1 = new Db(NULL, 0);
	db1->remove("table1.db", NULL, 0);
	delete this->db1;
	delete this->storage1;
	delete this->storage2;
	delete this->storage3;
	utils::FileSystem::rmDirRecursive(path1);
	utils::FileSystem::rmDirRecursive(path2);
	utils::FileSystem::rmDirRecursive(path3);
}

void KeyValueCountingBloomFilterTest::testConstructor() {
	bloom::KeyValueCountingBloomFilter Filter1(hashFunction_, *storage1, 10,
			false, 0.01);
	Filter1.AbstractBloomFilter::add("bla1");
	Filter1.AbstractBloomFilter::add("bla2");
	Filter1.AbstractBloomFilter::add("bla3");
	Filter1.AbstractBloomFilter::add("bla4");
	bloom::KeyValueCountingBloomFilter Filter2(hashFunction_, *storage1, 10,
			false, 0.01);
	CPPUNIT_ASSERT(Filter1.AbstractBloomFilter::contains("bla1"));
	CPPUNIT_ASSERT(Filter2.AbstractBloomFilter::contains("bla1"));
	CPPUNIT_ASSERT(Filter2.AbstractBloomFilter::contains("bla2"));
	CPPUNIT_ASSERT(Filter2.AbstractBloomFilter::contains("bla3"));
	CPPUNIT_ASSERT(Filter2.AbstractBloomFilter::contains("bla4"));
}
void KeyValueCountingBloomFilterTest::testLoad() {
	bloom::KeyValueCountingBloomFilter Filter1(hashFunction_, *storage1, 10,
			false, 0.01);
	bloom::KeyValueCountingBloomFilter Filter2(hashFunction_, *storage2, 10,
			false, 0.01);
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
void KeyValueCountingBloomFilterTest::testInsert() {
	bloom::KeyValueCountingBloomFilter Filter1(hashFunction_, *storage1, 10,
			false, 0.01);
	//	 test signature (const unsigned char* key)
	unsigned char cad1[hashFunction_.getHashSize()];
	hashFunction_(cad1, "ejemplo");
	Filter1.add(cad1);
	CPPUNIT_ASSERT(Filter1.contains(cad1));

	//	 test signature (const std::string& key)
	Filter1.AbstractBloomFilter::add(string("hello"));
	CPPUNIT_ASSERT( Filter1.AbstractBloomFilter::contains(string("hello")));

	//	 test signature (const char* key)

	Filter1.AbstractBloomFilter::add("bye");
	CPPUNIT_ASSERT_EQUAL(true, Filter1.AbstractBloomFilter::contains("bye"));

	/// Testing transaction code
	bloom::KeyValueCountingBloomFilter Filter3(hashFunction_, *storage3, 10,
			false, 0.01);
	Filter3.AbstractBloomFilter::add("bla1");
	Filter3.AbstractBloomFilter::add("bla2");
	Filter3.AbstractBloomFilter::add("bla3");
	CPPUNIT_ASSERT(Filter3.itemCount_== 3);

	/// Testing berkeley db code
	bloom::KeyValueCountingBloomFilter Filter4(hashFunction_, *bdbstorage, 10,
			false, 0.01);
	Filter4.AbstractBloomFilter::add("bla1");
	Filter4.AbstractBloomFilter::add("bla2");
	Filter4.AbstractBloomFilter::add("bla3");
	CPPUNIT_ASSERT(Filter4.itemCount_== 3);
}
void KeyValueCountingBloomFilterTest::testRemove() {
	//cout << endl;
	bloom::KeyValueCountingBloomFilter Filter1(hashFunction_, *storage1, 10,
			false, 0.01);
	bloom::KeyValueCountingBloomFilter Filter2(hashFunction_, *storage2, 10,
			false, 0.01);
	bloom::KeyValueCountingBloomFilter Filter3(hashFunction_, *storage3, 10,
			false, 0.01);
	//	 test signature (const unsigned char* key)

	unsigned char cad1[hashFunction_.getHashSize()];
	hashFunction_(cad1, "ejemplo");
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
	CPPUNIT_ASSERT(Filter1.AbstractBloomFilter::contains(string("hello")));
	CPPUNIT_ASSERT(Filter1.CountingBloomFilter::remove(string("hello")));
	CPPUNIT_ASSERT(!Filter1.AbstractBloomFilter::contains(string("hello")));
	//	 test signature (const char* key)
	CPPUNIT_ASSERT(!Filter1.CountingBloomFilter::remove("bye"));
	Filter1.AbstractBloomFilter::add("bye");
	CPPUNIT_ASSERT(Filter1.AbstractBloomFilter::contains("bye"));
	CPPUNIT_ASSERT(Filter1.CountingBloomFilter::remove("bye"));
	CPPUNIT_ASSERT(!Filter1.CountingBloomFilter::remove("bye"));
	Filter1.AbstractBloomFilter::add("bla1");
	Filter2.AbstractBloomFilter::add("bla1");
	Filter2.clear();
	Filter1.AbstractBloomFilter::add("bla2");
	Filter2.AbstractBloomFilter::add("bla2");
	Filter2.clear();
	Filter1.AbstractBloomFilter::add("bla3");
	Filter2.AbstractBloomFilter::add("bla3");
	Filter2.clear();
	CPPUNIT_ASSERT(Filter1.CountingBloomFilter::remove("bla1"));
	CPPUNIT_ASSERT(Filter1.CountingBloomFilter::remove("bla2"));
	CPPUNIT_ASSERT(Filter1.AbstractBloomFilter::contains("bla3"));
	CPPUNIT_ASSERT(Filter1.CountingBloomFilter::remove("bla3"));
	/// Testing transaction code
	Filter3.AbstractBloomFilter::add("bla1");
	Filter3.AbstractBloomFilter::add("bla2");
	Filter3.AbstractBloomFilter::add("bla3");
	CPPUNIT_ASSERT(Filter3.CountingBloomFilter::remove("bla1"));
	CPPUNIT_ASSERT(Filter3.CountingBloomFilter::remove("bla2"));
	CPPUNIT_ASSERT(Filter3.CountingBloomFilter::remove("bla3"));
}
void KeyValueCountingBloomFilterTest::testContains() {
	/* test signature (const std::string& key) const */
	/* test signature (const char* data, const std::size_t& length) const */
	bloom::KeyValueCountingBloomFilter Filter1(hashFunction_, *storage1, 8196);
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
void KeyValueCountingBloomFilterTest::testContainsAll() {
	bloom::KeyValueCountingBloomFilter Filter2(hashFunction_, *storage2, 8196);
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
void KeyValueCountingBloomFilterTest::testDiff() {
	bloom::KeyValueCountingBloomFilter FilterA(hashFunction_, *storage1, 8196);
	bloom::KeyValueCountingBloomFilter FilterB(hashFunction_, *storage2, 8196);
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

void KeyValueCountingBloomFilterTest::testToString() {
	bloom::KeyValueCountingBloomFilter FilterA(hashFunction_, *storage1, 10);
	bloom::KeyValueCountingBloomFilter FilterB(hashFunction_, *storage2, 10);
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
}

}
