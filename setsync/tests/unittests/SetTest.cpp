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
	size_t treecutsize = 2 * localset.getHashFunction().getHashSize();
	unsigned char treecut[treecutsize];
	unsigned char localroot[localset.getHashFunction().getHashSize()];
	unsigned char remoteroot[remoteset.getHashFunction().getHashSize()];
	SynchronizationProcess * localprocess = localset.createSyncProcess();
	setsync::ListDiffHandler localDiffHandler;
	SynchronizationProcess * remoteprocess = remoteset.createSyncProcess();
	setsync::ListDiffHandler remoteDiffHandler;
	CPPUNIT_ASSERT(localprocess->getRootHash(localroot));
	size_t subtriesize = localprocess->getSubTrie(localroot, treecut, treecutsize);
	remoteprocess->diffTrie(treecut,subtriesize,remoteDiffHandler);
	CPPUNIT_ASSERT(remoteDiffHandler.size() == 0);
	CPPUNIT_ASSERT(localDiffHandler.size() == 0);
	CPPUNIT_ASSERT(remoteprocess->getRootHash(remoteroot));
	subtriesize = remoteprocess->getSubTrie(remoteroot, treecut, treecutsize);
	localprocess->diffTrie(treecut, subtriesize, localDiffHandler);
	CPPUNIT_ASSERT(localDiffHandler.size() <= 2);
	CPPUNIT_ASSERT(localDiffHandler.size() > 0);
	localDiffHandler.clear();
	localset.insert("bla4");
	// Both Tries are equal
	CPPUNIT_ASSERT(localset == remoteset);

	localset.insert("bla5");
	localset.insert("bla6");
	localset.insert("bla8");
	localset.insert("bla9");
	localset.insert("bla10");

	remoteset.insert("bla7");
	remoteset.insert("bla11");
	remoteset.insert("bla12");
	remoteset.insert("bla13");
	remoteset.insert("bla14");

	localprocess->getRootHash(localroot);
	subtriesize = localprocess->getSubTrie(localroot, treecut, treecutsize);
	remoteprocess->diffTrie(treecut, subtriesize, remoteDiffHandler);
	for (size_t i = 0; i < remoteDiffHandler.size(); i++) {
		unsigned char subtrie[treecutsize];
		size_t entrysize = localprocess->getSubTrie(remoteDiffHandler[i].first, subtrie,
				treecutsize);
		size_t entries = entrysize / localset.getHashFunction().getHashSize();
		if (entries == 1) {
			remoteset.insert(subtrie);
		} else {
			remoteprocess->diffTrie(subtrie, entrysize, remoteDiffHandler);
		}
	}
	unsigned char temphash[localset.getHashFunction().getHashSize()];
	localset.getHashFunction()(temphash, "bla5");
	CPPUNIT_ASSERT(remoteset.find(temphash));
	remoteDiffHandler.clear();
	remoteprocess->getRootHash(remoteroot);
	subtriesize = remoteprocess->getSubTrie(remoteroot, treecut, treecutsize);
	localprocess->diffTrie(treecut, subtriesize, localDiffHandler);
	for (size_t i = 0; i < localDiffHandler.size(); i++) {
		unsigned char subtrie[treecutsize];
		size_t entrysize = remoteprocess->getSubTrie(localDiffHandler[i].first, subtrie,
				treecutsize);
		size_t entries = entrysize / localset.getHashFunction().getHashSize();
		if (entries == 1) {
			localset.insert(subtrie);
		} else {
			localprocess->diffTrie(subtrie, entrysize, localDiffHandler);
		}
	}
	localset.getHashFunction()(temphash, "bla6");
	CPPUNIT_ASSERT(localset.find(temphash));
	localset.getHashFunction()(temphash, "bla7");
	CPPUNIT_ASSERT(localset.find(temphash));
	CPPUNIT_ASSERT(localset == remoteset);

	delete localprocess;
	delete remoteprocess;
}

void SetTest::setUp() {
	this->dir = new utils::FileSystem::TemporaryDirectory("temp_");
	this->config.setPath(dir->getPath());
}
void SetTest::tearDown() {
	delete this->dir;
}
}
