/*
 * SetTest.cpp
 *
 *      Author: Till Lorentzen
 */

#include "SetTest.h"

using namespace std;

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
	localDiffHandler.clear();
	remoteDiffHandler.clear();
	// TRIE SYNC Process!!!
	size_t treecutsize = 2 * localset.getHashFunction().getHashSize();
	unsigned char treecut[treecutsize];
	size_t subtriesize;
	std::size_t ackbuffersize = 20;
	unsigned char ackbuffer[ackbuffersize];
	std::size_t numberOfAcks;
	std::size_t acksize;

	while (!localprocess->done() && !remoteprocess->done()) {
		if (localprocess->isSubtrieOutputAvailable()) {
			subtriesize = localprocess->getSubTrie(treecut, treecutsize);
			CPPUNIT_ASSERT(subtriesize > 0);
			acksize = remoteprocess->processSubTrie(treecut, subtriesize);
			CPPUNIT_ASSERT(acksize > 0);
		}
		if (localprocess->isAckOutputAvailable()) {
			acksize = localprocess->readSomeTrieAcks(ackbuffer, ackbuffersize,
					&numberOfAcks);
			CPPUNIT_ASSERT(numberOfAcks > 0);
			CPPUNIT_ASSERT(acksize > 0);
			remoteprocess->processAcks(ackbuffer, acksize, numberOfAcks,
					remoteDiffHandler);
		}
		if (remoteprocess->isSubtrieOutputAvailable()) {
			subtriesize = remoteprocess->getSubTrie(treecut, treecutsize);
			CPPUNIT_ASSERT(subtriesize > 0);
			acksize = localprocess->processSubTrie(treecut, subtriesize);
			CPPUNIT_ASSERT(acksize > 0);
		}
		if (remoteprocess->isAckOutputAvailable()) {
			acksize = remoteprocess->readSomeTrieAcks(ackbuffer, ackbuffersize,
					&numberOfAcks);
			CPPUNIT_ASSERT(numberOfAcks > 0);
			CPPUNIT_ASSERT(acksize > 0);
			localprocess->processAcks(ackbuffer, acksize, numberOfAcks,
					localDiffHandler);
		}
	}
	for (std::size_t i = 0; i < localDiffHandler.size(); i++) {
		remoteset.insert(localDiffHandler[i].first);
	}
	localDiffHandler.clear();
	for (std::size_t i = 0; i < remoteDiffHandler.size(); i++) {
		localset.insert(remoteDiffHandler[i].first);
	}
	remoteDiffHandler.clear();
	CPPUNIT_ASSERT(localprocess->getRootHash(buffer));
	CPPUNIT_ASSERT(remoteprocess->isEqual(buffer));
	CPPUNIT_ASSERT(remoteprocess->getRootHash(buffer));
	CPPUNIT_ASSERT(localprocess->isEqual(buffer));
	CPPUNIT_ASSERT(localprocess->getReceivedBytes() == remoteprocess->getSentBytes());
	CPPUNIT_ASSERT(localprocess->getSentBytes() == remoteprocess->getReceivedBytes());
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
	// Start sync
	size_t treecutsize = 2 * localset.getHashFunction().getHashSize();
	unsigned char treecut[treecutsize];
	SynchronizationProcess * localprocess = localset.createSyncProcess();
	setsync::ListDiffHandler localDiffHandler;
	SynchronizationProcess * remoteprocess = remoteset.createSyncProcess();
	setsync::ListDiffHandler remoteDiffHandler;

	size_t subtriesize;
	std::size_t ackbuffersize = 20;
	unsigned char ackbuffer[ackbuffersize];
	std::size_t numberOfAcks;
	std::size_t acksize;

	// Adding a greater diff to the sets
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
	// Deep search

	while (!localprocess->done() && !remoteprocess->done()) {
		if (localprocess->isSubtrieOutputAvailable()) {
			subtriesize = localprocess->getSubTrie(treecut, treecutsize);
			CPPUNIT_ASSERT(subtriesize > 0);
			acksize = remoteprocess->processSubTrie(treecut, subtriesize);
			CPPUNIT_ASSERT(acksize > 0);
		}
		if (localprocess->isAckOutputAvailable()) {
			acksize = localprocess->readSomeTrieAcks(ackbuffer, ackbuffersize,
					&numberOfAcks);
			CPPUNIT_ASSERT(numberOfAcks > 0);
			CPPUNIT_ASSERT(acksize > 0);
			remoteprocess->processAcks(ackbuffer, acksize, numberOfAcks,
					remoteDiffHandler);
		}
		if (remoteprocess->isSubtrieOutputAvailable()) {
			subtriesize = remoteprocess->getSubTrie(treecut, treecutsize);
			CPPUNIT_ASSERT(subtriesize > 0);
			acksize = localprocess->processSubTrie(treecut, subtriesize);
			CPPUNIT_ASSERT(acksize > 0);
		}
		if (remoteprocess->isAckOutputAvailable()) {
			acksize = remoteprocess->readSomeTrieAcks(ackbuffer, ackbuffersize,
					&numberOfAcks);
			CPPUNIT_ASSERT(numberOfAcks > 0);
			CPPUNIT_ASSERT(acksize > 0);
			localprocess->processAcks(ackbuffer, acksize, numberOfAcks,
					localDiffHandler);
		}
	}
	for (std::size_t i = 0; i < localDiffHandler.size(); i++) {
		remoteset.insert(localDiffHandler[i].first);
	}
	localDiffHandler.clear();
	CPPUNIT_ASSERT(remoteset.find("bla5"));
	CPPUNIT_ASSERT(remoteset.find("bla6"));
	CPPUNIT_ASSERT(remoteset.find("bla8"));
	CPPUNIT_ASSERT(remoteset.find("bla9"));
	CPPUNIT_ASSERT(remoteset.find("bla10"));

	for (std::size_t i = 0; i < remoteDiffHandler.size(); i++) {
		localset.insert(remoteDiffHandler[i].first);
	}
	remoteDiffHandler.clear();
	CPPUNIT_ASSERT(localset.find("bla7"));
	CPPUNIT_ASSERT(localset.find("bla11"));
	CPPUNIT_ASSERT(localset.find("bla12"));
	CPPUNIT_ASSERT(localset.find("bla13"));
	CPPUNIT_ASSERT(localset.find("bla14"));

	CPPUNIT_ASSERT(localset == remoteset);
	CPPUNIT_ASSERT(localprocess->getReceivedBytes() == remoteprocess->getSentBytes());
	CPPUNIT_ASSERT(localprocess->getSentBytes() == remoteprocess->getReceivedBytes());
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
