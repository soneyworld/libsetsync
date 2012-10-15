/*
 * FSTrieTest.cpp
 *
 *      Author: Till Lorentzen
 */

#include "DBTrieTest.h"
#include <sstream>


using namespace std;

namespace trie{

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

void DbTrieNodeTest::setUp(void) {
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
}

void DbTrieNodeTest::tearDown(void) {
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

void DbTrieNodeTest::testConstructor() {
	trie::DbNode node(this->db, key1, true);

}

void DbTrieNodeTest::testEquals() {
	throw "";
}
};
