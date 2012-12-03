/*
 * LevelDbTrieTest.cpp
 *
 *      Author: Till Lorentzen
 */

#include "LevelDbTrieTest.h"
#include <setsync/storage/LevelDbStorage.h>
#include <setsync/utils/FileSystem.h>
#include <sstream>
#include <setsync/utils/OutputFunctions.h>
#include <stdlib.h>
#include <setsync/DiffHandler.h>

using namespace std;

namespace trie {

void LevelDbTrieTest::setUp(void) {
	path1 = "trie1";
	path2 = "trie2";
	this->storage1 = new setsync::storage::LevelDbStorage(path1);
	this->storage2 = new setsync::storage::LevelDbStorage(path2);
	this->smaller = (unsigned char *) malloc(this->hash.getHashSize());
	this->larger = (unsigned char *) malloc(this->hash.getHashSize());
	memset(this->smaller, 0, hash.getHashSize());
	memset(this->larger, 0xFF, hash.getHashSize());
}

void LevelDbTrieTest::tearDown(void) {
	delete this->storage1;
	delete this->storage2;
	utils::FileSystem::rmDirRecursive(path1);
	utils::FileSystem::rmDirRecursive(path2);
}

void LevelDbTrieTest::testAdding() {
	trie::LevelDbTrie trie(hash, *storage1);
	CPPUNIT_ASSERT(trie.getSize() == 0);
	CPPUNIT_ASSERT(trie.Trie::add("bla1"));
	CPPUNIT_ASSERT(trie.getSize() == 1);
	CPPUNIT_ASSERT(!trie.Trie::add("bla1"));
	CPPUNIT_ASSERT(!trie.Trie::add(std::string("bla1")));
	CPPUNIT_ASSERT(trie.getSize() == 1);
	CPPUNIT_ASSERT(trie.Trie::add("bla2"));
	CPPUNIT_ASSERT(trie.getSize() == 2);
}

void LevelDbTrieTest::testAddingAndErasingElements() {
	trie::LevelDbTrie trie(hash, *storage1);
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

void LevelDbTrieTest::testAddingAndCleaningElements() {
	trie::LevelDbTrie trie(hash, *storage1);
	CPPUNIT_ASSERT(trie.getSize() == 0);
	CPPUNIT_ASSERT(trie.Trie::add("bla1"));
	CPPUNIT_ASSERT(trie.Trie::add("bla2"));
	CPPUNIT_ASSERT(trie.Trie::add("bla3"));
	CPPUNIT_ASSERT(trie.getSize() == 3);
	trie.clear();
	CPPUNIT_ASSERT(trie.getSize() == 0);
}

void LevelDbTrieTest::testContains() {
	trie::LevelDbTrie trie(hash, *storage1);
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

void LevelDbTrieTest::testSize() {
	trie::LevelDbTrie trie(hash, *storage1);
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

void LevelDbTrieTest::testEquals() {
	trie::LevelDbTrie trie1(hash, *storage1);
	trie::LevelDbTrie trie2(hash, *storage2);
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

void LevelDbTrieTest::testSavingAndLoading() {
	string path = "trieCopy";
	setsync::storage::LevelDbStorage * copy = new setsync::storage::LevelDbStorage(path);
	trie::LevelDbTrie * trie = new trie::LevelDbTrie(hash, *copy);
	CPPUNIT_ASSERT(trie->Trie::add("bla1"));
	CPPUNIT_ASSERT(trie->Trie::add("bla2"));
	delete trie;
	delete copy;
	copy = new setsync::storage::LevelDbStorage("trieCopy");
	trie = new trie::LevelDbTrie(hash, *copy);
	CPPUNIT_ASSERT(trie->Trie::contains("bla1"));
	CPPUNIT_ASSERT(trie->Trie::contains("bla2"));
	delete trie;
	delete copy;
	utils::FileSystem::rmDirRecursive(path);
}

void LevelDbTrieTest::testToString() {
	//	cout << endl;
	trie::LevelDbTrie trie1(hash, *storage1);
	for (unsigned int i = 1; i <= 10; i++) {
		stringstream ss;
		ss << "test" << i;
		CPPUNIT_ASSERT(trie1.Trie::add(ss.str()));
	}
	string dot = trie1.toString();
	//cout << dot;
}

void LevelDbTrieTest::testSubTrie() {
	trie::LevelDbTrie trie1(hash, *storage1);
	trie::LevelDbTrie trie2(hash, *storage2);
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
	TrieNode rootnode(trie2, hash, *storage2, root, false);
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

void LevelDbTrieTest::testDiff() {
	for (int iter = 2; iter <= 128; iter = iter * 2) {
		trie::LevelDbTrie trie1(hash, *storage1);
		trie::LevelDbTrie trie2(hash, *storage2);
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
