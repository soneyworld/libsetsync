/*
 * KeyValueTrieTest.cpp
 *
 *      Author: Till Lorentzen
 */

#include "KeyValueTrieTest.h"
#include <sstream>
#include <setsync/utils/OutputFunctions.h>
#include <stdlib.h>
#include <setsync/DiffHandler.h>

using namespace std;

namespace trie {

void KeyValueTrieSyncTest::setUp(void) {
	this->db1 = new Db(NULL, 0);
	this->db2 = new Db(NULL, 0);
	db1->open(NULL, "table1.db", NULL, DB_HASH, DB_CREATE, 0);
	db2->open(NULL, "table2.db", NULL, DB_HASH, DB_CREATE, 0);
	this->storage1 = new setsync::storage::BdbStorage(this->db1);
	this->storage2 = new setsync::storage::BdbStorage(this->db2);
	this->trie1 = new KeyValueTrie(hashFunction_, *storage1);
	this->trie2 = new KeyValueTrie(hashFunction_, *storage2);
	this->process1 = this->trie1->createSyncProcess();
	this->process2 = this->trie2->createSyncProcess();
}

void KeyValueTrieSyncTest::tearDown(void) {
	delete this->process1;
	delete this->process2;
	delete this->trie1;
	delete this->trie2;
	delete this->storage1;
	delete this->storage2;
	this->db1->close(0);
	this->db2->close(0);
	delete this->db1;
	delete this->db2;
	this->db1 = new Db(NULL, 0);
	this->db2 = new Db(NULL, 0);
	db1->remove("table1.db", NULL, 0);
	db2->remove("table2.db", NULL, 0);
	delete this->db1;
	delete this->db2;
}

void KeyValueTrieSyncTest::testInput() {
	throw "not yet implemented";
}

void KeyValueTrieSyncTest::testOutput() {
	throw "not yet implemented";
}

void KeyValueTrieSyncTest::testSync() {
	CPPUNIT_ASSERT(*trie1 == *trie2);
	unsigned char hash[hashFunction_.getHashSize()];
	hashFunction_(hash, "bla1");
	this->trie1->add(hash);
	hashFunction_(hash, "bla2");
	this->trie1->add(hash);
	hashFunction_(hash, "bla3");
	this->trie2->add(hash);
	CPPUNIT_ASSERT(*trie1 != *trie2);
	std::size_t buffersize = 20;
	unsigned char buffer[buffersize];
	while (this->process1->pendingOutput() || this->process2->pendingOutput()
			|| this->process1->awaitingInput()
			|| this->process2->awaitingInput()) {
		if (this->process1->awaitingInput()) {
			CPPUNIT_ASSERT(this->process2->pendingOutput());
			setsync::ListDiffHandler handler;
			while (this->process2->pendingOutput()) {
				this->process2->writeOutput(buffer, buffersize);
				this->process1->processInput(buffer, buffersize, handler);
			}
			for (std::size_t i = 0; i < handler.size(); i++) {
				this->trie2->add(handler[i].first);
			}
		} else if (this->process2->awaitingInput()) {
			CPPUNIT_ASSERT(this->process1->pendingOutput());
			setsync::ListDiffHandler handler;
			while (this->process1->pendingOutput()) {
				this->process1->writeOutput(buffer, buffersize);
				this->process2->processInput(buffer, buffersize, handler);
			}
			for (std::size_t i = 0; i < handler.size(); i++) {
				this->trie1->add(handler[i].first);
			}
		} else if (this->process1->pendingOutput()) {
			setsync::ListDiffHandler handler;
			while (this->process1->pendingOutput()) {
				this->process1->writeOutput(buffer, buffersize);
				this->process2->processInput(buffer, buffersize, handler);
			}
			for (std::size_t i = 0; i < handler.size(); i++) {
				this->trie1->add(handler[i].first);
			}
		} else if (this->process2->pendingOutput()) {
			setsync::ListDiffHandler handler;
			while (this->process2->pendingOutput()) {
				this->process2->writeOutput(buffer, buffersize);
				this->process1->processInput(buffer, buffersize, handler);
			}
			for (std::size_t i = 0; i < handler.size(); i++) {
				this->trie2->add(handler[i].first);
			}
		}
	}
	CPPUNIT_ASSERT(*trie1 == *trie2);
}

void KeyValueTrieTest::setUp(void) {
	this->db = new Db(NULL, 0);
	db->open(NULL, "trie.db", "trie", DB_HASH, DB_CREATE, 0);
	this->db2 = new Db(NULL, 0);
	db2->open(NULL, "trie2.db", "trie", DB_HASH, DB_CREATE, 0);
	this->smaller = (unsigned char *) malloc(this->hash.getHashSize());
	this->larger = (unsigned char *) malloc(this->hash.getHashSize());
	memset(this->smaller, 0, hash.getHashSize());
	memset(this->larger, 0xFF, hash.getHashSize());
	this->storage1 = new setsync::storage::BdbStorage(this->db);
	this->storage2 = new setsync::storage::BdbStorage(this->db2);
}

void KeyValueTrieTest::tearDown(void) {
	delete this->storage1;
	delete this->storage2;
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

void KeyValueTrieTest::testAdding() {
	trie::KeyValueTrie trie(hash, *storage1);
	CPPUNIT_ASSERT(trie.getSize() == 0);
	CPPUNIT_ASSERT(trie.Trie::add("bla1"));
	CPPUNIT_ASSERT(trie.getSize() == 1);
	CPPUNIT_ASSERT(!trie.Trie::add("bla1"));
	CPPUNIT_ASSERT(!trie.Trie::add(std::string("bla1")));
	CPPUNIT_ASSERT(trie.getSize() == 1);
	CPPUNIT_ASSERT(trie.Trie::add("bla2"));
	CPPUNIT_ASSERT(trie.getSize() == 2);
}

void KeyValueTrieTest::testAddingAndErasingElements() {
	trie::KeyValueTrie trie(hash, *storage1);
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

void KeyValueTrieTest::testAddingAndCleaningElements() {
	trie::KeyValueTrie trie(hash, *storage1);
	CPPUNIT_ASSERT(trie.getSize() == 0);
	CPPUNIT_ASSERT(trie.Trie::add("bla1"));
	CPPUNIT_ASSERT(trie.Trie::add("bla2"));
	CPPUNIT_ASSERT(trie.Trie::add("bla3"));
	CPPUNIT_ASSERT(trie.getSize() == 3);
	trie.clear();
	CPPUNIT_ASSERT(trie.getSize() == 0);
}

void KeyValueTrieTest::testContains() {
	trie::KeyValueTrie trie(hash, *storage1);
	CPPUNIT_ASSERT(!trie.Trie::contains("bla1"));
	CPPUNIT_ASSERT(trie.Trie::contains("bla1") == NOT_FOUND);
	CPPUNIT_ASSERT(trie.Trie::add("bla1"));
	CPPUNIT_ASSERT(trie.Trie::contains("bla1"));
	CPPUNIT_ASSERT(trie.Trie::contains("bla1") == LEAF_NODE);
	CPPUNIT_ASSERT(!trie.Trie::contains("bla2"));
	CPPUNIT_ASSERT(trie.Trie::add("bla2"));
	CPPUNIT_ASSERT(trie.Trie::contains("bla1"));
	CPPUNIT_ASSERT(trie.Trie::contains("bla2"));
	unsigned char root[hash.getHashSize()];
	trie.getRoot(root);
	CPPUNIT_ASSERT(trie.contains(root) == INNER_NODE);
	CPPUNIT_ASSERT(trie.Trie::remove("bla1"));
	CPPUNIT_ASSERT(!trie.Trie::contains("bla1"));
}

void KeyValueTrieTest::testSize() {
	trie::KeyValueTrie trie(hash, *storage1);
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
		CPPUNIT_ASSERT(trie.getSize() == i - 1);
		CPPUNIT_ASSERT(trie.Trie::add(ss.str()));
		CPPUNIT_ASSERT(trie.getSize() == i);
	}

	for (unsigned int i = 100; i >= 1; i--) {
		stringstream ss;
		ss << "bla" << i;
		CPPUNIT_ASSERT(trie.getSize() == i);
		CPPUNIT_ASSERT(trie.Trie::remove(ss.str()));
		CPPUNIT_ASSERT(trie.getSize() == i - 1);
	}
}

void KeyValueTrieTest::testEquals() {
	trie::KeyValueTrie trie1(hash, *storage1);
	trie::KeyValueTrie trie2(hash, *storage2);
	CPPUNIT_ASSERT(trie1 == trie2);
	CPPUNIT_ASSERT(trie1.Trie::add("bla1"));
	CPPUNIT_ASSERT(!(trie1 == trie2));
	CPPUNIT_ASSERT(trie2.Trie::add("bla1"));
	CPPUNIT_ASSERT(trie1 == trie2);
	CPPUNIT_ASSERT(trie1.Trie::add("bla2"));
	CPPUNIT_ASSERT(!(trie1 == trie2));
	CPPUNIT_ASSERT(trie2.Trie::add("bla2"));
	CPPUNIT_ASSERT(trie1 == trie2);
	CPPUNIT_ASSERT(trie1.Trie::add("bla3"));
	CPPUNIT_ASSERT(trie1.Trie::add("bla4"));
	CPPUNIT_ASSERT(trie1.Trie::add("bla5"));
	CPPUNIT_ASSERT(trie1.Trie::add("bla6"));
	CPPUNIT_ASSERT(trie2.Trie::add("bla6"));
	CPPUNIT_ASSERT(trie2.Trie::add("bla5"));
	CPPUNIT_ASSERT(trie2.Trie::add("bla4"));
	CPPUNIT_ASSERT(trie2.Trie::add("bla3"));
	CPPUNIT_ASSERT(trie1 == trie2);
}

void KeyValueTrieTest::testSavingAndLoading() {
	Db * dbcopy = new Db(NULL, 0);
	dbcopy->open(NULL, "trieCopy.db", "trie", DB_HASH, DB_CREATE, 0);
	setsync::storage::BdbStorage * copystorage =
			new setsync::storage::BdbStorage(dbcopy);
	trie::KeyValueTrie * trie = new trie::KeyValueTrie(hash, *copystorage);
	CPPUNIT_ASSERT(trie->Trie::add("bla1"));
	CPPUNIT_ASSERT(trie->Trie::add("bla2"));
	delete trie;
	delete copystorage;
	dbcopy->close(0);
	delete dbcopy;
	dbcopy = new Db(NULL, 0);
	dbcopy->open(NULL, "trieCopy.db", "trie", DB_HASH, DB_CREATE, 0);
	copystorage = new setsync::storage::BdbStorage(dbcopy);
	trie = new trie::KeyValueTrie(hash, *copystorage);
	CPPUNIT_ASSERT(trie->Trie::contains("bla1"));
	CPPUNIT_ASSERT(trie->Trie::contains("bla2"));
	delete trie;
	delete copystorage;
	dbcopy->close(0);
	delete dbcopy;
	dbcopy = new Db(NULL, 0);
	dbcopy->remove("trieCopy.db", NULL, 0);
}

void KeyValueTrieTest::testToString() {
	//	cout << endl;
	trie::KeyValueTrie trie1(hash, *storage1);
	for (unsigned int i = 1; i <= 10; i++) {
		stringstream ss;
		ss << "test" << i;
		CPPUNIT_ASSERT(trie1.Trie::add(ss.str()));
	}
	string dot = trie1.toString();
	//cout << dot;
}

void KeyValueTrieTest::testSubTrie() {
	trie::KeyValueTrie trie1(hash, *storage1);
	trie::KeyValueTrie trie2(hash, *storage2);
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
	TrieNode rootnode = trie2.root_->get();
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
	trie1.clear();
	for (int i = 0; i < 20; i++) {
		stringstream ss;
		ss << "test" << i;
		trie1.Trie::add(ss.str());
	}
	trie1.getRoot(root);
	for (int i = 11; i <= 20; i++) {
		unsigned char buffer[i * hash.getHashSize()];
		size_t foundnodes = trie1.getSubTrie(root, buffer,
				i * hash.getHashSize()) / hash.getHashSize();
		CPPUNIT_ASSERT(foundnodes == (size_t) i);
		for (size_t j = 0; j < foundnodes; j++) {
			CPPUNIT_ASSERT(trie1.contains(buffer + j * hash.getHashSize()));
		}
	}

}

void KeyValueTrieTest::testDiff() {
	for (int iter = 2; iter <= 128; iter = iter * 2) {
		trie::KeyValueTrie trie1(hash, *storage1);
		trie::KeyValueTrie trie2(hash, *storage2);
		trie1.Trie::add("bla1");
		trie2.Trie::add("bla1");
		trie1.Trie::add("bla2");
		trie2.Trie::add("bla2");
		trie1.Trie::add("bla3");
		trie2.Trie::add("bla3");
		trie2.Trie::add("bla4");
		setsync::ListDiffHandler difflist;
		size_t treecutsize = iter * this->hash.getHashSize();
		unsigned char treecut[treecutsize];
		unsigned char root[this->hash.getHashSize()];
		trie1.getRoot(root);
		size_t subtriesize = trie1.getSubTrie(root, treecut, treecutsize);
		trie2.diff(treecut, subtriesize, difflist);
		CPPUNIT_ASSERT(difflist.size() == 0);
		trie2.getRoot(root);
		subtriesize = trie2.getSubTrie(root, treecut, treecutsize);
		trie1.diff(treecut, subtriesize, difflist);
		CPPUNIT_ASSERT(difflist.size() <= 2);
		CPPUNIT_ASSERT(difflist.size() > 0);
		difflist.clear();
		trie1.Trie::add("bla4");
		// Both Tries are equal
		CPPUNIT_ASSERT(trie1 == trie2);

		trie1.Trie::add("bla5");
		trie1.Trie::add("bla6");
		trie1.Trie::add("bla8");
		trie1.Trie::add("bla9");
		trie1.Trie::add("bla10");

		trie2.Trie::add("bla7");
		trie2.Trie::add("bla11");
		trie2.Trie::add("bla12");
		trie2.Trie::add("bla13");
		trie2.Trie::add("bla14");

		trie1.getRoot(root);
		subtriesize = trie1.getSubTrie(root, treecut, treecutsize);
		trie2.diff(treecut, subtriesize, difflist);
		for (size_t i = 0; i < difflist.size(); i++) {
			//			if (trie1.contains(difflist[i].first)) {
			unsigned char subtrie[treecutsize];
			size_t entrysize = trie1.getSubTrie(difflist[i].first, subtrie,
					treecutsize);
			size_t entries = entrysize / hash.getHashSize();
			if (entries == 1) {
				trie2.add(subtrie, true);
			} else {
				trie2.diff(subtrie, entrysize, difflist);
			}
			//			}
		}
		unsigned char temphash[hash.getHashSize()];
		hash(temphash, "bla5");
		CPPUNIT_ASSERT(trie2.contains(temphash));
		difflist.clear();
		trie2.getRoot(root);
		subtriesize = trie2.getSubTrie(root, treecut, treecutsize);
		trie1.diff(treecut, subtriesize, difflist);
		for (size_t i = 0; i < difflist.size(); i++) {
			//			if (trie2.contains(difflist[i].first)) {
			unsigned char subtrie[treecutsize];
			size_t entrysize = trie2.getSubTrie(difflist[i].first, subtrie,
					treecutsize);
			size_t entries = entrysize / hash.getHashSize();
			if (entries == 1) {
				trie1.add(subtrie, true);
			} else {
				trie1.diff(subtrie, entrysize, difflist);
			}
			//			}
		}
		hash(temphash, "bla6");
		CPPUNIT_ASSERT(trie1.contains(temphash));
		hash(temphash, "bla7");
		CPPUNIT_ASSERT(trie1.contains(temphash));
		CPPUNIT_ASSERT(trie1 == trie2);
		trie1.clear();
		trie2.clear();
	}
}

}
