/*
 * DBTrieTest.cpp
 *
 *      Author: Till Lorentzen
 */

#include "DBTrieTest.h"
#include <sstream>
#include <setsync/utils/OutputFunctions.h>
#include <stdlib.h>
using namespace std;

namespace trie {

void DbTrieTest::setUp(void) {
	this->db = new Db(NULL, 0);
	db->open(NULL, "trie.db", trie::DBTrie::getLogicalDatabaseName(),
			trie::DBTrie::getTableType(), DB_CREATE, 0);
	this->db2 = new Db(NULL, 0);
	db2->open(NULL, "trie2.db", trie::DBTrie::getLogicalDatabaseName(),
			trie::DBTrie::getTableType(), DB_CREATE, 0);
	this->smaller = (unsigned char *) malloc(this->hash.getHashSize());
	this->larger = (unsigned char *) malloc(this->hash.getHashSize());
	memset(this->smaller, 0, hash.getHashSize());
	memset(this->larger, 0xFF, hash.getHashSize());
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
	free(this->smaller);
	free(this->larger);
}

void DbTrieTest::testAdding() {
	trie::DBTrie trie(hash, db);
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
	trie::DBTrie trie(hash, db);
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
	trie::DBTrie trie(hash, db);
	CPPUNIT_ASSERT(trie.getSize() == 0);
	CPPUNIT_ASSERT(trie.Trie::add("bla1"));
	CPPUNIT_ASSERT(trie.Trie::add("bla2"));
	CPPUNIT_ASSERT(trie.Trie::add("bla3"));
	CPPUNIT_ASSERT(trie.getSize() == 3);
	trie.clear();
	CPPUNIT_ASSERT(trie.getSize() == 0);
}

void DbTrieTest::testContains() {
	trie::DBTrie trie(hash, db);
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
	trie::DBTrie trie(hash, db);
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
	trie::DBTrie trie1(hash, db);
	trie::DBTrie trie2(hash, db2);
	CPPUNIT_ASSERT(trie1==trie2);
	CPPUNIT_ASSERT(trie1.Trie::add("bla1"));
	CPPUNIT_ASSERT(!(trie1==trie2));
	CPPUNIT_ASSERT(trie2.Trie::add("bla1"));
	CPPUNIT_ASSERT(trie1==trie2);
	CPPUNIT_ASSERT(trie1.Trie::add("bla2"));
	CPPUNIT_ASSERT(!(trie1==trie2));
	CPPUNIT_ASSERT(trie2.Trie::add("bla2"));
	CPPUNIT_ASSERT(trie1==trie2);
	CPPUNIT_ASSERT(trie1.Trie::add("bla3"));
	CPPUNIT_ASSERT(trie1.Trie::add("bla4"));
	CPPUNIT_ASSERT(trie1.Trie::add("bla5"));
	CPPUNIT_ASSERT(trie1.Trie::add("bla6"));
	CPPUNIT_ASSERT(trie2.Trie::add("bla6"));
	CPPUNIT_ASSERT(trie2.Trie::add("bla5"));
	CPPUNIT_ASSERT(trie2.Trie::add("bla4"));
	CPPUNIT_ASSERT(trie2.Trie::add("bla3"));
	CPPUNIT_ASSERT(trie1==trie2);
}

void DbTrieTest::testSavingAndLoading() {
	Db * dbcopy = new Db(NULL, 0);
	dbcopy->open(NULL, "trieCopy.db", trie::DBTrie::getLogicalDatabaseName(),
			trie::DBTrie::getTableType(), DB_CREATE, 0);
	trie::DBTrie * trie = new trie::DBTrie(hash, dbcopy);
	CPPUNIT_ASSERT(trie->Trie::add("bla1"));
	CPPUNIT_ASSERT(trie->Trie::add("bla2"));
	delete trie;
	dbcopy->close(0);
	delete dbcopy;
	dbcopy = new Db(NULL, 0);
	dbcopy->open(NULL, "trieCopy.db", trie::DBTrie::getLogicalDatabaseName(),
			trie::DBTrie::getTableType(), DB_CREATE, 0);
	trie = new trie::DBTrie(hash, dbcopy);
	CPPUNIT_ASSERT(trie->Trie::contains("bla1"));
	CPPUNIT_ASSERT(trie->Trie::contains("bla2"));
	delete trie;
	dbcopy->close(0);
	delete dbcopy;
	dbcopy = new Db(NULL, 0);
	dbcopy->remove("trieCopy.db", NULL, 0);
}

void DbTrieTest::testToString() {
	//	cout << endl;
	trie::DBTrie trie1(hash, db);
	for (unsigned int i = 1; i <= 10; i++) {
		stringstream ss;
		ss << "test" << i;
		CPPUNIT_ASSERT(trie1.Trie::add(ss.str()));
	}
	string dot = trie1.toString();
	//cout << dot;
}

void DbTrieTest::testSubTrie() {
	trie::DBTrie trie1(hash, db);
	trie::DBTrie trie2(hash, db2);
	trie1.Trie::add("bla1");
	trie2.Trie::add("bla1");
	trie1.Trie::add("bla2");
	trie2.Trie::add("bla2");
	trie1.Trie::add("bla3");
	trie2.Trie::add("bla3");
	trie2.Trie::add("bla4");
	//	cout << endl;
	//	cout << trie1.toString() << endl;
	//	cout << trie2.toString() << endl;
	size_t buffersize = 100 * hash.getHashSize();
	unsigned char buffer[buffersize];
	unsigned char root[hash.getHashSize()];
	CPPUNIT_ASSERT(trie1.getRoot(root));
	size_t subtrie = trie1.getSubTrie(root, buffer, buffersize);
	CPPUNIT_ASSERT(subtrie == 3 * hash.getHashSize());
	CPPUNIT_ASSERT(trie2.getRoot(root));
	subtrie = trie2.getSubTrie(root, buffer, buffersize);
	CPPUNIT_ASSERT(subtrie == 4 * hash.getHashSize());
	unsigned char temphash[hash.getHashSize()];
	hash(temphash, "bla1");
	bool found = false;
	for (size_t i = 0; i < subtrie / hash.getHashSize(); i++) {
		if (memcmp(buffer + (i * hash.getHashSize()), temphash,
				hash.getHashSize()) == 0) {
			found = true;
			break;
		}
	}
	CPPUNIT_ASSERT(found);
	hash(temphash, "bla2");
	found = false;
	for (size_t i = 0; i < subtrie / hash.getHashSize(); i++) {
		if (memcmp(buffer + (i * hash.getHashSize()), temphash,
				hash.getHashSize()) == 0) {
			found = true;
			break;
		}
	}
	CPPUNIT_ASSERT(found);
	hash(temphash, "bla3");
	found = false;
	for (size_t i = 0; i < subtrie / hash.getHashSize(); i++) {
		if (memcmp(buffer + (i * hash.getHashSize()), temphash,
				hash.getHashSize()) == 0) {
			found = true;
			break;
		}
	}
	CPPUNIT_ASSERT(found);
	hash(temphash, "bla4");
	found = false;
	for (size_t i = 0; i < subtrie / hash.getHashSize(); i++) {
		if (memcmp(buffer + (i * hash.getHashSize()), temphash,
				hash.getHashSize()) == 0) {
			found = true;
			break;
		}
	}
	CPPUNIT_ASSERT(found);
	subtrie = trie2.getSubTrie(root, buffer, 2 * hash.getHashSize());
	DbNode rootnode(trie2, hash, db2, root, false);
//	cout << endl << trie2.toString() << endl;
//	cout << utils::OutputFunctions::CryptoHashtoString(buffer) << endl;
//	cout << utils::OutputFunctions::CryptoHashtoString(
//			buffer + hash.getHashSize()) << endl;
	CPPUNIT_ASSERT(subtrie == 2 * hash.getHashSize());
	found = false;
	for (size_t i = 0; i < subtrie / hash.getHashSize(); i++) {
		if (memcmp(buffer + (i * hash.getHashSize()), rootnode.smaller,
				hash.getHashSize()) == 0) {
			found = true;
			break;
		}
	}
	CPPUNIT_ASSERT(found);
	found = false;
	for (size_t i = 0; i < subtrie / hash.getHashSize(); i++) {
		if (memcmp(buffer + (i * hash.getHashSize()), rootnode.larger,
				hash.getHashSize()) == 0) {
			found = true;
			break;
		}
	}
	CPPUNIT_ASSERT(found);

}

void DbNodeTest::setUp(void) {
	this->db = new Db(NULL, 0);
	db->open(NULL, "trienode1.db", trie::DBTrie::getLogicalDatabaseName(),
			trie::DBTrie::getTableType(), DB_CREATE, 0);
	this->db2 = new Db(NULL, 0);
	db2->open(NULL, "trienode2.db", trie::DBTrie::getLogicalDatabaseName(),
			trie::DBTrie::getTableType(), DB_CREATE, 0);
	this->trie = new DBTrie(hashFunction_, this->db);
	this->key1 = (unsigned char*) malloc(hashFunction_.getHashSize());
	this->key2 = (unsigned char*) malloc(hashFunction_.getHashSize());
	this->key3 = (unsigned char*) malloc(hashFunction_.getHashSize());
	this->key4 = (unsigned char*) malloc(hashFunction_.getHashSize());
	this->smaller = (unsigned char*) malloc(hashFunction_.getHashSize());
	this->larger = (unsigned char*) malloc(hashFunction_.getHashSize());
	this->hash = (unsigned char*) malloc(hashFunction_.getHashSize());
	hashFunction_(key1, "bla1");
	hashFunction_(key2, "bla2");
	hashFunction_(key3, "bla3");
	hashFunction_(key4, "bla4");
	memset(this->smaller, 0, hashFunction_.getHashSize());
	memset(this->larger, 0xFF, hashFunction_.getHashSize());
}

void DbNodeTest::tearDown(void) {
	delete this->trie;
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
	free(key1);
	free(key2);
	free(key3);
	free(key4);
	free(smaller);
	free(larger);
	free(hash);
}

void DbNodeTest::testConstructor() {
	DbNode node1(*trie, hashFunction_, this->db, key1, true);
	CPPUNIT_ASSERT_EQUAL(memcmp(node1.hash,key1,hashFunction_.getHashSize()),0);
	CPPUNIT_ASSERT_EQUAL(memcmp(node1.prefix,key1,hashFunction_.getHashSize()),0);
	CPPUNIT_ASSERT_EQUAL(node1.hasParent_,false);
	CPPUNIT_ASSERT_EQUAL(node1.hasChildren_,false);
	node1.toDb();
	DbNode node2(*trie, hashFunction_, this->db, key1, false);
	CPPUNIT_ASSERT(node1==node2);
	DbNode node3(*trie, hashFunction_, this->db, key1);
	CPPUNIT_ASSERT(node2==node3);
	DbNode node4 = node1;
	CPPUNIT_ASSERT(node1==node4);
	DbNode node5(node1);
	CPPUNIT_ASSERT(node1==node5);
}

void DbNodeTest::testEquals() {
	DbNode node1(*trie, hashFunction_, this->db, key1, true);
	node1.toDb();
	CPPUNIT_ASSERT(node1==node1);
	DbNode node2(*trie, hashFunction_, this->db, key1, true);
	CPPUNIT_ASSERT(node1==node2);
	DbNode node3(*trie, hashFunction_, this->db, key1);
	CPPUNIT_ASSERT(node1==node3);
	CPPUNIT_ASSERT(node2==node3);
}

void DbNodeTest::testNotEquals() {
	DbNode node1(*trie, hashFunction_, this->db, key1, true);
	DbNode node2(*trie, hashFunction_, this->db, key2, true);
	DbNode node3(*trie, hashFunction_, this->db, key1, true);
	CPPUNIT_ASSERT(node1!=node2);
	CPPUNIT_ASSERT(node2!=node3);
	CPPUNIT_ASSERT(!(node1!=node1));
	CPPUNIT_ASSERT(!(node1!=node3));
}

void DbNodeTest::testLarger() {
	DbNode node1(*trie, hashFunction_, this->db, smaller, true);
	node1.prefix_mask = 1;
	DbNode node2(*trie, hashFunction_, this->db, larger, true);
	node2.prefix_mask = 1;
	CPPUNIT_ASSERT(node2 > node1);
	CPPUNIT_ASSERT(node1.isLarger(node2));
	CPPUNIT_ASSERT(!node2.isLarger(node1));
	CPPUNIT_ASSERT(!node1.isSmaller(node2));
	CPPUNIT_ASSERT(node2.isSmaller(node1));
	CPPUNIT_ASSERT(!(node1 > node2));
	CPPUNIT_ASSERT(node1 < node2);
	CPPUNIT_ASSERT(!(node2 < node1));
}

void DbNodeTest::testUpdateHash() {
	DbNode node1(*trie, hashFunction_, this->db, key1, true);
	memcpy(node1.smaller, smaller, hashFunction_.getHashSize());
	memcpy(node1.larger, larger, hashFunction_.getHashSize());
	node1.hasChildren_ = true;
	node1.updateHash();
	unsigned char scratch[hashFunction_.getHashSize() * 2];
	memcpy(scratch, smaller, hashFunction_.getHashSize());
	memcpy(scratch + hashFunction_.getHashSize(), larger,
			hashFunction_.getHashSize());
	hashFunction_(this->hash, scratch, hashFunction_.getHashSize() * 2);
	CPPUNIT_ASSERT_EQUAL(memcmp(node1.hash, this->hash, hashFunction_.getHashSize()), 0);
}

void DbNodeTest::testInsert() {
	DbNode rootnode(*trie, hashFunction_, this->db, smaller, true);
	rootnode.toDb();
	DbRootNode root(*trie, hashFunction_, this->db);
	root.set(smaller);
	CPPUNIT_ASSERT(rootnode.hasChildren_ == false);
	CPPUNIT_ASSERT(rootnode.hasChildren() == false);
	CPPUNIT_ASSERT(rootnode.hasParent_ == false);
	CPPUNIT_ASSERT(rootnode.hasParent() == false);
	rootnode = root.get();
	CPPUNIT_ASSERT(rootnode.insert(larger));
	rootnode = root.get();
	CPPUNIT_ASSERT(rootnode.hasChildren_ == true);
	CPPUNIT_ASSERT(rootnode.hasParent_ == false);
	DbNode smaller = rootnode.getSmaller();
	DbNode larger = rootnode.getLarger();
	CPPUNIT_ASSERT(smaller.hasChildren_ == false);
	CPPUNIT_ASSERT(smaller.hasParent_ == true);
	CPPUNIT_ASSERT(larger.hasChildren_ == false);
	CPPUNIT_ASSERT(larger.hasParent_ == true);
	CPPUNIT_ASSERT(!rootnode.insert(larger));
	CPPUNIT_ASSERT(rootnode.insert(key1));
	rootnode = root.get();
	CPPUNIT_ASSERT(rootnode.hasChildren_ == true);
	CPPUNIT_ASSERT(rootnode.hasParent_ == false);
	CPPUNIT_ASSERT(rootnode.insert(key2));
	rootnode = root.get();
	CPPUNIT_ASSERT(rootnode.hasChildren_ == true);
	CPPUNIT_ASSERT(rootnode.hasParent_ == false);
}

void DbNodeTest::testErase() {
	DBTrie trie1(hashFunction_, this->db);
	// Inserting 30 entries
	for (int i = 0; i < 30; i++) {
		stringstream ss;
		ss << "bla" << i;
		CPPUNIT_ASSERT(trie1.Trie::add(ss.str()));
	}
	// Removing 30 entries without hashing
	for (int i = 29; i >= 0; i--) {
		stringstream ss;
		ss << "bla" << i;
		bool success = trie1.Trie::remove(ss.str(), false);
		CPPUNIT_ASSERT(success);
	}
	// Inserting 30 entries
	for (int i = 0; i < 30; i++) {
		stringstream ss;
		ss << "bla" << i;
		CPPUNIT_ASSERT(trie1.Trie::add(ss.str()));
	}
	// Removing 30 entries with hashing
	for (int i = 29; i >= 0; i--) {
		stringstream ss;
		ss << "bla" << i;
		bool success = trie1.Trie::remove(ss.str(), true);
		CPPUNIT_ASSERT(success);
	}

	DBTrie trie2(hashFunction_, this->db2);
	CPPUNIT_ASSERT_MESSAGE("BOTH SHOULD BE EMPTY", trie1==trie2);
	CPPUNIT_ASSERT_MESSAGE("adding bla1 to emtpy trie1 failed", trie1.Trie::add("bla1"));
	CPPUNIT_ASSERT_MESSAGE("trie1: bla1 added", !(trie1==trie2));
	CPPUNIT_ASSERT_MESSAGE("adding new key bla2 to trie1 failed",trie1.Trie::add("bla2"));
	CPPUNIT_ASSERT_MESSAGE("trie1: bla2 added", !(trie1==trie2));
	CPPUNIT_ASSERT_MESSAGE("adding new key bla3 to trie1 failed",trie1.Trie::add("bla3"));
	CPPUNIT_ASSERT_MESSAGE("trie1: bla3 added", !(trie1==trie2));
	CPPUNIT_ASSERT_MESSAGE("adding new key bla1 to trie2 failed", trie2.Trie::add("bla1"));
	CPPUNIT_ASSERT_MESSAGE("trie2: bla1 added", !(trie1==trie2));
	CPPUNIT_ASSERT_MESSAGE("adding new key bla2 to trie2 failed",trie2.Trie::add("bla2"));
	CPPUNIT_ASSERT_MESSAGE("trie2: bla2 added",!(trie1==trie2));
	CPPUNIT_ASSERT_MESSAGE("removing key bla3 from trie1 failed",trie1.Trie::remove("bla3"));
	CPPUNIT_ASSERT_MESSAGE("Both trie should contain: bla1, bla2", trie1==trie2);
}

void DbNodeTest::testCommon() {
	DbNode node1(*trie, hashFunction_, this->db, key1, true);
	DbNode node2(*trie, hashFunction_, this->db, key1, true);
	CPPUNIT_ASSERT(node1.commonPrefixSize(node2)==8*hashFunction_.getHashSize());
	DbNode node3(*trie, hashFunction_, this->db, smaller, true);
	DbNode node4(*trie, hashFunction_, this->db, larger, true);
	CPPUNIT_ASSERT(node3.commonPrefixSize(node4)==0);
	DbNode node5(*trie, hashFunction_, this->db, smaller, true);
	memset(node5.prefix + hashFunction_.getHashSize() / 2, 0x1,
			hashFunction_.getHashSize() / 2);
	CPPUNIT_ASSERT(node3.commonPrefixSize(node5)==4*hashFunction_.getHashSize());
}

void DbNodeTest::testSimilar() {
	DbNode smaller(*trie, hashFunction_, this->db, this->smaller, true);
	DbNode larger(*trie, hashFunction_, this->db, this->larger, true);
	smaller.prefix_mask = 0;
	larger.prefix_mask = 0;
	CPPUNIT_ASSERT(smaller.similar(larger));
	CPPUNIT_ASSERT(larger.similar(smaller));
	smaller.prefix_mask = 1;
	larger.prefix_mask = 1;
	CPPUNIT_ASSERT(!smaller.similar(larger));
	CPPUNIT_ASSERT(!larger.similar(smaller));
	smaller.prefix_mask = 0;
	larger.prefix_mask = 1;
	CPPUNIT_ASSERT(smaller.similar(larger));
	CPPUNIT_ASSERT(!larger.similar(smaller));

}

void DbNodeTest::testToDb() {
	DbNode node1(*trie, hashFunction_, this->db, key1, true);
	DbNode node2(*trie, hashFunction_, this->db, key2, true);
	DbNode node3(*trie, hashFunction_, this->db, key3, true);
	DbNode node4(*trie, hashFunction_, this->db, key4, true);
	node1.dirty_ = true;
	node2.hasChildren_ = true;
	node3.hasParent_ = true;
	node4.prefix_mask = 8;
	CPPUNIT_ASSERT(node1.toDb());
	CPPUNIT_ASSERT(node2.toDb());
	CPPUNIT_ASSERT(node3.toDb());
	CPPUNIT_ASSERT(node4.toDb());
	DbNode node5(*trie, hashFunction_, this->db, key1);
	DbNode node6(*trie, hashFunction_, this->db, key2);
	DbNode node7(*trie, hashFunction_, this->db, key3);
	DbNode node8(*trie, hashFunction_, this->db, key4);
	CPPUNIT_ASSERT(node1 == node5);
	CPPUNIT_ASSERT(node2 == node6);
	CPPUNIT_ASSERT(node3 == node7);
	CPPUNIT_ASSERT(node4 == node8);
	CPPUNIT_ASSERT(node2 != node5);
	CPPUNIT_ASSERT(node3 != node6);
	CPPUNIT_ASSERT(node4 != node7);
	CPPUNIT_ASSERT(node1 != node8);
}

void DbNodeTest::testToString() {
	//	cout << endl;
	//	cout << "KEY1: " << utils::OutputFunctions::CryptoHashtoString(key1) << endl;
	//	cout << "KEY2: " << utils::OutputFunctions::CryptoHashtoString(key2) << endl;
	DbNode node1(*trie, hashFunction_, this->db, smaller, true);
	node1.toDb();
	//	cout << node1.toString();
	DbRootNode root(*trie, hashFunction_, this->db);
	root.set(smaller);
	node1 = root.get();
	//	cout << node1.toString();
	node1.insert(larger);
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
	DBTrie trie(hash, db);
	DbRootNode root(trie, hash, this->db);
	root.toString();
	//	std::cout << root.toString() << std::endl;
}
}
