/*
 * DBTrieTest.cpp
 *
 *      Author: Till Lorentzen
 */

#include "DBTrieTest.h"
#include <sstream>
#include <setsync/utils/OutputFunctions.h>
using namespace std;

namespace trie {

void DbTrieTest::setUp(void) {
	this->db = new Db(NULL, 0);
	db->open(NULL, "trie.db", trie::DBTrie::getLogicalDatabaseName(),
			trie::DBTrie::getTableType(), DB_CREATE, 0);
	this->db2 = new Db(NULL, 0);
	db2->open(NULL, "trie2.db", trie::DBTrie::getLogicalDatabaseName(),
			trie::DBTrie::getTableType(), DB_CREATE, 0);
}

void DbTrieTest::tearDown(void) {
	db->close(0);
	db2->close(0);
	delete this->db;
	delete this->db2;
	this->db = new Db(NULL, 0);
	this->db->remove("trie.db", NULL, 0);
	delete this->db;
	this->db = new Db(NULL, 0);
	this->db->remove("trie2.db", NULL, 0);
	delete this->db;
	remove("trieCopy.db");
}

void DbTrieTest::testAdding() {
	trie::DBTrie trie(db);
	CPPUNIT_ASSERT(trie.getSize() == 0);
	CPPUNIT_ASSERT(trie.Trie::add("bla1"));
	CPPUNIT_ASSERT(trie.getSize() == 1);
	CPPUNIT_ASSERT(!trie.Trie::add("bla1"));
	CPPUNIT_ASSERT(!trie.Trie::add(std::string("bla1")));
	CPPUNIT_ASSERT(trie.getSize() == 1);
	CPPUNIT_ASSERT(trie.Trie::add("bla2"));
	CPPUNIT_ASSERT(trie.getSize() == 2);
}

void DbTrieTest::testAddingAndErasingElements() {
	trie::DBTrie trie(db);
	CPPUNIT_ASSERT(trie.getSize() == 0);
	CPPUNIT_ASSERT(trie.Trie::add("bla1"));
	CPPUNIT_ASSERT(trie.getSize() == 1);
	CPPUNIT_ASSERT(trie.Trie::remove("bla1"));
	CPPUNIT_ASSERT(trie.getSize() == 0);
	CPPUNIT_ASSERT(!trie.Trie::remove("bla1"));
	CPPUNIT_ASSERT(trie.getSize() == 0);
	CPPUNIT_ASSERT(trie.Trie::add("bla1"));
	CPPUNIT_ASSERT(trie.Trie::add("bla2"));
	CPPUNIT_ASSERT(trie.getSize() == 2);
	CPPUNIT_ASSERT(trie.Trie::remove("bla1"));
	CPPUNIT_ASSERT(!trie.Trie::remove("bla1"));
	CPPUNIT_ASSERT(trie.getSize() == 1);
	CPPUNIT_ASSERT(trie.Trie::remove("bla2"));
	CPPUNIT_ASSERT(trie.getSize() == 0);
	CPPUNIT_ASSERT(!trie.Trie::remove("bla2"));
	CPPUNIT_ASSERT(trie.getSize() == 0);
}

void DbTrieTest::testAddingAndCleaningElements() {
	trie::DBTrie trie(db);
	CPPUNIT_ASSERT(trie.getSize() == 0);
	CPPUNIT_ASSERT(trie.Trie::add("bla1"));
	CPPUNIT_ASSERT(trie.Trie::add("bla2"));
	CPPUNIT_ASSERT(trie.Trie::add("bla3"));
	CPPUNIT_ASSERT(trie.getSize() == 3);
	trie.clear();
	CPPUNIT_ASSERT(trie.getSize() == 0);
}

void DbTrieTest::testContains() {
	trie::DBTrie trie(db);
	CPPUNIT_ASSERT(!trie.Trie::contains("bla1"));
	CPPUNIT_ASSERT(trie.Trie::add("bla1"));
	CPPUNIT_ASSERT(trie.Trie::contains("bla1"));
	CPPUNIT_ASSERT(!trie.Trie::contains("bla2"));
	CPPUNIT_ASSERT(trie.Trie::add("bla2"));
	CPPUNIT_ASSERT(trie.Trie::contains("bla1"));
	CPPUNIT_ASSERT(trie.Trie::contains("bla2"));
	CPPUNIT_ASSERT(trie.Trie::remove("bla1"));
	CPPUNIT_ASSERT(!trie.Trie::contains("bla1"));
}

void DbTrieTest::testSize() {
	trie::DBTrie trie(db);
	CPPUNIT_ASSERT(trie.getSize() == 0);
	CPPUNIT_ASSERT(trie.Trie::add("bla1"));
	CPPUNIT_ASSERT(trie.getSize() == 1);
	CPPUNIT_ASSERT(trie.Trie::remove("bla1"));
	CPPUNIT_ASSERT(trie.getSize() == 0);
	CPPUNIT_ASSERT(!trie.Trie::remove("bla1"));
	CPPUNIT_ASSERT(trie.getSize() == 0);
	CPPUNIT_ASSERT(trie.Trie::add("bla1"));
	CPPUNIT_ASSERT(trie.Trie::add("bla2"));
	CPPUNIT_ASSERT(trie.getSize() == 2);
	CPPUNIT_ASSERT(trie.Trie::remove("bla1"));
	CPPUNIT_ASSERT(!trie.Trie::remove("bla1"));
	CPPUNIT_ASSERT(trie.getSize() == 1);
	CPPUNIT_ASSERT(trie.Trie::remove("bla2"));
	CPPUNIT_ASSERT(trie.getSize() == 0);
	CPPUNIT_ASSERT(!trie.Trie::remove("bla2"));
	CPPUNIT_ASSERT(trie.getSize() == 0);

	for (unsigned int i = 1; i <= 100; i++) {
		stringstream ss;
		ss << "bla" << i;
		CPPUNIT_ASSERT(trie.getSize()==i-1);
		CPPUNIT_ASSERT(trie.Trie::add(ss.str()));
		CPPUNIT_ASSERT(trie.getSize()==i);
	}

	for (unsigned int i = 100; i >= 1; i--) {
		stringstream ss;
		ss << "bla" << i;
		CPPUNIT_ASSERT(trie.getSize()==i);
		CPPUNIT_ASSERT(trie.Trie::remove(ss.str()));
		CPPUNIT_ASSERT(trie.getSize()==i-1);
	}
}

void DbTrieTest::testEquals() {
	trie::DBTrie trie1(db);
	trie::DBTrie trie2(db2);
	CPPUNIT_ASSERT(trie1==trie2);
	CPPUNIT_ASSERT(trie1.Trie::add("bla1"));
	CPPUNIT_ASSERT(!(trie1==trie2));
	CPPUNIT_ASSERT(trie2.Trie::add("bla1"));
	CPPUNIT_ASSERT(trie1==trie2);
	CPPUNIT_ASSERT(trie1.Trie::add("bla2"));
	CPPUNIT_ASSERT(!(trie1==trie2));
	CPPUNIT_ASSERT(trie2.Trie::add("bla2"));
	CPPUNIT_ASSERT(trie1==trie2);
}

void DbTrieTest::testSavingAndLoading() {
	Db * dbcopy = new Db(NULL, 0);
	dbcopy->open(NULL, "trieCopy.db", trie::DBTrie::getLogicalDatabaseName(),
			trie::DBTrie::getTableType(), DB_CREATE, 0);
	trie::DBTrie * trie = new trie::DBTrie(dbcopy);
	CPPUNIT_ASSERT(trie->Trie::add("bla1"));
	CPPUNIT_ASSERT(trie->Trie::add("bla2"));
	delete trie;
	dbcopy->close(0);
	delete dbcopy;
	dbcopy = new Db(NULL, 0);
	dbcopy->open(NULL, "trieCopy.db", trie::DBTrie::getLogicalDatabaseName(),
			trie::DBTrie::getTableType(), DB_CREATE, 0);
	trie = new trie::DBTrie(dbcopy);
	CPPUNIT_ASSERT(trie->Trie::contains("bla1"));
	CPPUNIT_ASSERT(trie->Trie::contains("bla2"));
	delete trie;
	dbcopy->close(0);
	delete dbcopy;
	dbcopy = new Db(NULL, 0);
	dbcopy->remove("trieCopy.db", NULL, 0);
}

void DbNodeTest::setUp(void) {
	this->db = new Db(NULL, 0);
	db->open(NULL, "trienode1.db", trie::DBTrie::getLogicalDatabaseName(),
			trie::DBTrie::getTableType(), DB_CREATE, 0);
	this->db2 = new Db(NULL, 0);
	db2->open(NULL, "trienode2.db", trie::DBTrie::getLogicalDatabaseName(),
			trie::DBTrie::getTableType(), DB_CREATE, 0);
	SHA1((unsigned char*) "bla1", 4, this->key1);
	SHA1((unsigned char*) "bla2", 4, this->key2);
	SHA1((unsigned char*) "bla3", 4, this->key3);
	SHA1((unsigned char*) "bla4", 4, this->key4);
	memset(this->smaller, 0, HASHSIZE);
	memset(this->larger, 0xFF, HASHSIZE);
}

void DbNodeTest::tearDown(void) {
	db->close(0);
	db2->close(0);
	delete this->db;
	delete this->db2;
	this->db = new Db(NULL, 0);
	this->db->remove("trienode1.db", NULL, 0);
	delete this->db;
	this->db = new Db(NULL, 0);
	this->db->remove("trienode2.db", NULL, 0);
	delete this->db;
}

void DbNodeTest::testConstructor() {
	DbNode node1(this->db, key1, true);
	CPPUNIT_ASSERT_EQUAL(memcmp(node1.hash,key1,HASHSIZE),0);
	CPPUNIT_ASSERT_EQUAL(memcmp(node1.prefix,key1,HASHSIZE),0);
	CPPUNIT_ASSERT_EQUAL(node1.hasParent_,false);
	CPPUNIT_ASSERT_EQUAL(node1.hasChildren_,false);
	node1.toDb();
	DbNode node2(this->db, key1, false);
	CPPUNIT_ASSERT(node1==node2);
	DbNode node3(this->db, key1);
	CPPUNIT_ASSERT(node2==node3);
	DbNode node4 = node1;
	CPPUNIT_ASSERT(node1==node4);
	DbNode node5(node1);
	CPPUNIT_ASSERT(node1==node5);
}

void DbNodeTest::testEquals() {
	DbNode node1(this->db, key1, true);
	node1.toDb();
	CPPUNIT_ASSERT(node1==node1);
	DbNode node2(this->db, key1, true);
	CPPUNIT_ASSERT(node1==node2);
	DbNode node3(this->db, key1);
	CPPUNIT_ASSERT(node1==node3);
	CPPUNIT_ASSERT(node2==node3);
}

void DbNodeTest::testNotEquals() {
	DbNode node1(this->db, key1, true);
	DbNode node2(this->db, key2, true);
	DbNode node3(this->db, key1, true);
	CPPUNIT_ASSERT(node1!=node2);
	CPPUNIT_ASSERT(node2!=node3);
	CPPUNIT_ASSERT(!(node1!=node1));
	CPPUNIT_ASSERT(!(node1!=node3));
}

void DbNodeTest::testLarger() {
	DbNode node1(this->db, smaller, true);
	node1.prefix_mask = 10;
	DbNode node2(this->db, larger, true);
	node2.prefix_mask = 10;
	CPPUNIT_ASSERT(node2 > node1);
	CPPUNIT_ASSERT(!(node1 > node2));
	CPPUNIT_ASSERT(node1 < node2);
	CPPUNIT_ASSERT(!(node2 < node1));
}

void DbNodeTest::testUpdateHash() {
	DbNode node1(this->db, key1, true);
	memcpy(node1.smaller, smaller, HASHSIZE);
	memcpy(node1.larger, larger, HASHSIZE);
	node1.hasChildren_ = true;
	node1.updateHash();
	unsigned char scratch[HASHSIZE * 2];
	memcpy(scratch, smaller, HASHSIZE);
	memcpy(scratch + HASHSIZE, larger, HASHSIZE);
	SHA1(scratch, HASHSIZE * 2, this->hash);
	CPPUNIT_ASSERT_EQUAL(memcmp(node1.hash, this->hash, HASHSIZE), 0);
}

void DbNodeTest::testInsert() {
	DbNode node1(this->db, key1, true);
	node1.toDb();
	DbRootNode root(this->db);
	root.set(key1);
	CPPUNIT_ASSERT(node1.hasChildren_ == false);
	CPPUNIT_ASSERT(node1.hasChildren() == false);
	CPPUNIT_ASSERT(node1.hasParent_ == false);
	CPPUNIT_ASSERT(node1.hasParent() == false);
	CPPUNIT_ASSERT(root.get().insert(key2));
	CPPUNIT_ASSERT(!root.get().insert(key2));
}

void DbNodeTest::testErase() {
	throw "";
}

void DbNodeTest::testCommon() {
	DbNode node1(this->db, key1, true);
	DbNode node2(this->db, key1, true);
	CPPUNIT_ASSERT(node1.commonPrefixSize(node2)==8*HASHSIZE);
	DbNode node3(this->db, smaller, true);
	DbNode node4(this->db, larger, true);
	CPPUNIT_ASSERT(node3.commonPrefixSize(node4)==0);
	DbNode node5(this->db, smaller, true);
	memset(node5.prefix + HASHSIZE / 2, 0x1, HASHSIZE / 2);
	CPPUNIT_ASSERT(node3.commonPrefixSize(node5)==4*HASHSIZE);
}

void DbNodeTest::testToDb() {
	DbNode node1(this->db, key1, true);
	DbNode node2(this->db, key1, true);
	DbNode node3(this->db, key3, true);
	CPPUNIT_ASSERT(node1.toDb());
	CPPUNIT_ASSERT(node2.toDb());
	CPPUNIT_ASSERT(node3.toDb());
	DbNode node4(this->db, key1);
	CPPUNIT_ASSERT(node4 == node1);
}

void DbNodeTest::testToString() {
//	cout << endl;
//	cout << "KEY1: " << utils::OutputFunctions::CryptoHashtoString(key1) << endl;
//	cout << "KEY2: " << utils::OutputFunctions::CryptoHashtoString(key2) << endl;
	DbNode node1(this->db, key1, true);
	node1.toDb();
//	cout << node1.toString();
	DbRootNode root(this->db);
	root.set(key1);
	node1 = root.get();
//	cout << node1.toString();
	node1.insert(key2);
//	cout << node1.toString();
	node1 = root.get();
//	cout << node1.toString();
//	std::cout << std::endl;
}

void DbRootNodeTest::setUp(void) {
	this->db = new Db(NULL, 0);
	db->open(NULL, "trierootnode1.db", trie::DBTrie::getLogicalDatabaseName(),
			trie::DBTrie::getTableType(), DB_CREATE, 0);
	this->db2 = new Db(NULL, 0);
	db2->open(NULL, "trierootnode2.db", trie::DBTrie::getLogicalDatabaseName(),
			trie::DBTrie::getTableType(), DB_CREATE, 0);
	SHA1((unsigned char*) "bla1", 4, this->key1);
	SHA1((unsigned char*) "bla2", 4, this->key2);
	SHA1((unsigned char*) "bla3", 4, this->key3);
	SHA1((unsigned char*) "bla4", 4, this->key4);
}

void DbRootNodeTest::tearDown(void) {
	db->close(0);
	db2->close(0);
	delete this->db;
	delete this->db2;
	this->db = new Db(NULL, 0);
	this->db->remove("trierootnode1.db", NULL, 0);
	delete this->db;
	this->db = new Db(NULL, 0);
	this->db->remove("trierootnode2.db", NULL, 0);
	delete this->db;
}

void DbRootNodeTest::testToDb() {

}

void DbRootNodeTest::testConstructor() {

}

void DbRootNodeTest::testToString() {
	DbRootNode root(this->db);
	root.toString();
//	std::cout << root.toString() << std::endl;
}
}
