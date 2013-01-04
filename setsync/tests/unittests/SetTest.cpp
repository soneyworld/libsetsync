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

void SetTest::testSync() {
	setsync::Set localset(config);
	setsync::Set remoteset(config2);
	CPPUNIT_ASSERT(localset.insert("hallo"));
	CPPUNIT_ASSERT(remoteset.insert("hallo"));

	CPPUNIT_ASSERT(localset.insert("hallo1"));
	SynchronizationProcess * localprocess = localset.createSyncProcess();
	setsync::ListDiffHandler localDiffHandler;
	SynchronizationProcess * remoteprocess = remoteset.createSyncProcess();
	setsync::ListDiffHandler remoteDiffHandler;
	std::size_t buffersize = localset.getHashFunction().getHashSize();
	unsigned char buffer[buffersize];
	std::size_t sending;
	while (localprocess->isBloomFilterOutputAvail()) {
		sending = localprocess->readSomeBloomFilter(buffer, buffersize);
		remoteprocess->diffBloomFilter(buffer, sending, remoteDiffHandler);
		if (remoteprocess->isBloomFilterOutputAvail()) {
			sending = remoteprocess->readSomeBloomFilter(buffer, buffersize);
			localprocess->diffBloomFilter(buffer, sending, localDiffHandler);
		}
	}
	while (remoteprocess->isBloomFilterOutputAvail()) {
		sending = remoteprocess->readSomeBloomFilter(buffer, buffersize);
		localprocess->diffBloomFilter(buffer, sending, localDiffHandler);
	}
	for (size_t i = 0; i < localDiffHandler.size(); i++) {
		CPPUNIT_ASSERT(remoteset.insert(localDiffHandler[i].first));
	}
	for (size_t i = 0; i < remoteDiffHandler.size(); i++) {
		CPPUNIT_ASSERT(localset.insert(remoteDiffHandler[i].first));
	}
	// At this moment, the sets could be equal, but to ensure,
	// they are not, we will just put in a new entry to local set
	CPPUNIT_ASSERT(localset.insert("hallo2"));
	CPPUNIT_ASSERT(!(localset == remoteset));
	// TRIE SYNC Process!!!
	CPPUNIT_ASSERT(localprocess->getRootHash(buffer));
	CPPUNIT_ASSERT(remoteprocess->isEqual(buffer));
	CPPUNIT_ASSERT(remoteprocess->getRootHash(buffer));
	CPPUNIT_ASSERT(localprocess->isEqual(buffer));
	delete localprocess;
	delete remoteprocess;
	CPPUNIT_ASSERT(localset == remoteset);
}

void SetTest::testStartSync() {
	setsync::Set localset(config);
	setsync::Set remoteset(config2);
	CPPUNIT_ASSERT(localset.insert("hallo"));
	CPPUNIT_ASSERT(remoteset.insert("hallo"));
	CPPUNIT_ASSERT(localset == remoteset);
	SynchronizationProcess * localprocess = localset.createSyncProcess();
	setsync::ListDiffHandler localDiffHandler;
	SynchronizationProcess * remoteprocess = remoteset.createSyncProcess();
	setsync::ListDiffHandler remoteDiffHandler;
	std::size_t buffersize = localset.hash_.getHashSize();
	unsigned char buffer[buffersize];
	std::size_t sending;
	CPPUNIT_ASSERT(localprocess->getRootHash(buffer));
	CPPUNIT_ASSERT(remoteprocess->isEqual(buffer));
	CPPUNIT_ASSERT(remoteprocess->getRootHash(buffer));
	CPPUNIT_ASSERT(localprocess->isEqual(buffer));
	CPPUNIT_ASSERT(localDiffHandler.size()==0);
	CPPUNIT_ASSERT(remoteDiffHandler.size()==0);
	delete localprocess;
	delete remoteprocess;
	CPPUNIT_ASSERT(localset == remoteset);
}

void SetTest::testLooseSync() {
	setsync::Set localset(config);
	setsync::Set remoteset(config2);
	CPPUNIT_ASSERT(localset.insert("hallo"));
	CPPUNIT_ASSERT(remoteset.insert("hallo"));

	CPPUNIT_ASSERT(localset.insert("hallo1"));
	SynchronizationProcess * localprocess = localset.createSyncProcess();
	setsync::ListDiffHandler localDiffHandler;
	SynchronizationProcess * remoteprocess = remoteset.createSyncProcess();
	setsync::ListDiffHandler remoteDiffHandler;
	std::size_t buffersize = localset.getHashFunction().getHashSize();
	unsigned char buffer[buffersize];
	std::size_t sending;
	while (localprocess->isBloomFilterOutputAvail()) {
		sending = localprocess->readSomeBloomFilter(buffer, buffersize);
		remoteprocess->diffBloomFilter(buffer, sending, remoteDiffHandler);
		if (remoteprocess->isBloomFilterOutputAvail()) {
			sending = remoteprocess->readSomeBloomFilter(buffer, buffersize);
			localprocess->diffBloomFilter(buffer, sending, localDiffHandler);
		}
	}
	while (remoteprocess->isBloomFilterOutputAvail()) {
		sending = remoteprocess->readSomeBloomFilter(buffer, buffersize);
		localprocess->diffBloomFilter(buffer, sending, localDiffHandler);
	}
	for (size_t i = 0; i < localDiffHandler.size(); i++) {
		CPPUNIT_ASSERT(remoteset.insert(localDiffHandler[i].first));
	}
	for (size_t i = 0; i < remoteDiffHandler.size(); i++) {
		CPPUNIT_ASSERT(localset.insert(remoteDiffHandler[i].first));
	}
	CPPUNIT_ASSERT(localset == remoteset);
	delete localprocess;
	delete remoteprocess;
}

void SetTest::testStrictSync() {
	setsync::Set localset(config);
	setsync::Set remoteset(config2);
	localset.insert("bla1");
	remoteset.insert("bla1");
	localset.insert("bla2");
	remoteset.insert("bla2");
	localset.insert("bla3");
	remoteset.insert("bla3");
	remoteset.insert("bla4");
	setsync::ListDiffHandler difflist;
	size_t treecutsize = 2 * localset.getHashFunction().getHashSize();
	unsigned char treecut[treecutsize];
	unsigned char root[localset.getHashFunction().getHashSize()];
	SynchronizationProcess * localprocess = localset.createSyncProcess();
	setsync::ListDiffHandler localDiffHandler;
	SynchronizationProcess * remoteprocess = remoteset.createSyncProcess();
	setsync::ListDiffHandler remoteDiffHandler;
	size_t subtriesize = trie1.getSubTrie(root, treecut, treecutsize);
	trie2.diff(treecut, subtriesize, difflist);
	CPPUNIT_ASSERT(difflist.size() == 0);
	trie2.getRoot(root);
	subtriesize = trie2.getSubTrie(root, treecut, treecutsize);
	trie1.diff(treecut, subtriesize, difflist);
	CPPUNIT_ASSERT(difflist.size() <= 2);
	CPPUNIT_ASSERT(difflist.size() > 0);
	difflist.clear();
	localset.add("bla4");
	// Both Tries are equal
	CPPUNIT_ASSERT(trie1 == trie2);

	localset.add("bla5");
	localset.add("bla6");
	localset.add("bla8");
	localset.add("bla9");
	localset.add("bla10");

	remoteset.add("bla7");
	remoteset.add("bla11");
	remoteset.add("bla12");
	remoteset.add("bla13");
	remoteset.add("bla14");

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

void SetTest::setUp() {
	this->dir = new utils::FileSystem::TemporaryDirectory("temp_");
	this->config.setPath(dir->getPath());
}
void SetTest::tearDown() {
	delete this->dir;
}
}
