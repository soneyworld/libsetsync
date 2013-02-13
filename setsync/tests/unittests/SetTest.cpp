/*
 * SetTest.cpp
 *
 *      Author: Till Lorentzen
 */

#include "SetTest.h"

using namespace std;

void localset_callback(void *closure, const unsigned char * hash,
		const size_t hashsize, const size_t existsLocally) {
	SET * set = (SET *) closure;
	set_insert(set, hash);
}

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

	CPPUNIT_ASSERT(localprocess->calcOutputBufferSize(1,1024*8)>=localprocess->getMinBuffer());
	CPPUNIT_ASSERT(localprocess->calcOutputBufferSize(1000000,1024*8) == 1024);
	unsigned char buffer[buffersize];
	std::size_t sending;
	while (localprocess->isBloomFilterOutputAvail()) {
		sending = localprocess->readNextBloomFilterChunk(buffer, buffersize);
		remoteprocess->processBloomFilterChunk(buffer, sending, remoteDiffHandler);
		if (remoteprocess->isBloomFilterOutputAvail()) {
			sending = remoteprocess->readNextBloomFilterChunk(buffer, buffersize);
			localprocess->processBloomFilterChunk(buffer, sending, localDiffHandler);
		}
	}
	while (remoteprocess->isBloomFilterOutputAvail()) {
		sending = remoteprocess->readNextBloomFilterChunk(buffer, buffersize);
		localprocess->processBloomFilterChunk(buffer, sending, localDiffHandler);
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
		sending = localprocess->readNextBloomFilterChunk(buffer, buffersize);
		remoteprocess->processBloomFilterChunk(buffer, sending, remoteDiffHandler);
		if (remoteprocess->isBloomFilterOutputAvail()) {
			sending = remoteprocess->readNextBloomFilterChunk(buffer, buffersize);
			localprocess->processBloomFilterChunk(buffer, sending, localDiffHandler);
		}
	}
	while (remoteprocess->isBloomFilterOutputAvail()) {
		sending = remoteprocess->readNextBloomFilterChunk(buffer, buffersize);
		localprocess->processBloomFilterChunk(buffer, sending, localDiffHandler);
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

void SetTest::testCAPI() {
	SET localset;
	SET remoteset;
	SET_CONFIG localconfig = set_create_config();
	SET_CONFIG remoteconfig = set_create_config();
	CPPUNIT_ASSERT(set_init(&localset,localconfig) == 0);
	CPPUNIT_ASSERT(set_init(&remoteset,remoteconfig) == 0);
	CPPUNIT_ASSERT(set_empty(&localset));
	CPPUNIT_ASSERT(set_empty(&remoteset));
	CPPUNIT_ASSERT(set_max_size(&localset) == localconfig.bf_max_elements);
	CPPUNIT_ASSERT(set_max_size(&remoteset) == remoteconfig.bf_max_elements);
	CPPUNIT_ASSERT(set_size(&localset) == 0);
	CPPUNIT_ASSERT(set_size(&remoteset) == 0);
	CPPUNIT_ASSERT(set_insert_string(&localset, "bla1"));
	CPPUNIT_ASSERT(set_insert_string(&remoteset, "bla2"));
	CPPUNIT_ASSERT(set_size(&localset) == 1);
	CPPUNIT_ASSERT(set_size(&remoteset) == 1);
	CPPUNIT_ASSERT(set_find_string(&localset, "bla1"));
	CPPUNIT_ASSERT(set_find_string(&remoteset, "bla2"));
	CPPUNIT_ASSERT(!set_find_string(&localset, "bla2"));
	CPPUNIT_ASSERT(!set_find_string(&remoteset, "bla1"));

	CPPUNIT_ASSERT(set_insert_string(&localset, "bla3"));
	CPPUNIT_ASSERT(set_size(&localset) == 2);
	CPPUNIT_ASSERT(set_erase_string(&localset, "bla3"));
	CPPUNIT_ASSERT(set_size(&localset) == 1);
	SET_SYNC_HANDLE localprocess;
	SET_SYNC_HANDLE remoteprocess;
	set_sync_init_handle(&localset, &localprocess);
	set_sync_init_handle(&remoteset, &remoteprocess);

	CPPUNIT_ASSERT(!set_sync_done(&localprocess));
	CPPUNIT_ASSERT(!set_sync_done(&remoteprocess));


	CPPUNIT_ASSERT(set_sync_min_bf_buffer(&localprocess)==1);
	CPPUNIT_ASSERT(set_sync_min_trie_buffer(&localprocess)==20*2);
	CPPUNIT_ASSERT(set_sync_min_trie_buffer(&localprocess)<=set_sync_min_buffer(&localprocess));

	// BF Sync
	std::size_t buffersize = 100;
	unsigned char buffer[buffersize];
	std::size_t sending;
	diff_callback * callback = &localset_callback;
	while (set_sync_bf_output_avail(&localprocess) == 1) {
		sending = set_sync_bf_read_next_chunk(&localprocess, buffer, buffersize);
		set_sync_bf_process_chunk(&remoteprocess, buffer, sending, callback, &localset);
	}
	while (set_sync_bf_output_avail(&remoteprocess) == 1) {
		sending = set_sync_bf_read_next_chunk(&remoteprocess, buffer, buffersize);
		set_sync_bf_process_chunk(&localprocess, buffer, sending, callback, &remoteset);
	}

	// BF SYNC DONE AND SET SHOULD BE EQUAL, SO ADDING A DIFF
	CPPUNIT_ASSERT(set_insert_string(&localset, "diff"));

	// Trie Sync
	size_t treecutsize = 2 * 20;
	unsigned char treecut[treecutsize];
	size_t subtriesize;
	std::size_t ackbuffersize = 20;
	unsigned char ackbuffer[ackbuffersize];
	std::size_t numberOfAcks;
	std::size_t acksize;

	while (!set_sync_done(&localprocess) && !set_sync_done(&remoteprocess)) {
		if (set_sync_trie_subtrie_output_avail(&localprocess)) {
			subtriesize = set_sync_trie_get_subtrie(&localprocess, treecut,
					treecutsize);
			CPPUNIT_ASSERT(subtriesize > 0);
			acksize = set_sync_trie_process_subtrie(&remoteprocess, treecut,
					subtriesize);
			CPPUNIT_ASSERT(acksize > 0);
		}
		if (set_sync_trie_acks_avail(&localprocess)) {
			acksize = set_sync_trie_read_acks(&localprocess, ackbuffer,
					ackbuffersize, &numberOfAcks);
			CPPUNIT_ASSERT(numberOfAcks > 0);
			CPPUNIT_ASSERT(acksize > 0);
			set_sync_trie_process_acks(&remoteprocess, ackbuffer, acksize,
					numberOfAcks, callback, &localset);
		}
		if (set_sync_trie_subtrie_output_avail(&remoteprocess)) {
			subtriesize = set_sync_trie_get_subtrie(&remoteprocess, treecut,
					treecutsize);
			CPPUNIT_ASSERT(subtriesize > 0);
			acksize = set_sync_trie_process_subtrie(&localprocess, treecut,
					subtriesize);
			CPPUNIT_ASSERT(acksize > 0);
		}
		if (set_sync_trie_acks_avail(&remoteprocess)) {
			acksize = set_sync_trie_read_acks(&remoteprocess, ackbuffer,
					ackbuffersize, &numberOfAcks);
			CPPUNIT_ASSERT(numberOfAcks > 0);
			CPPUNIT_ASSERT(acksize > 0);
			set_sync_trie_process_acks(&localprocess, ackbuffer, acksize,
					numberOfAcks, callback, &remoteset);
		}
	}
	// TRIES SHOULD BE EQUAL -> TEST IT
	unsigned char localroot[20];
	unsigned char remoteroot[20];
	CPPUNIT_ASSERT(set_sync_get_root_hash(&localprocess, localroot)==0);
	CPPUNIT_ASSERT(set_sync_get_root_hash(&remoteprocess, remoteroot)==0);
	CPPUNIT_ASSERT(set_sync_is_equal_to_hash(&localprocess,remoteroot));
	CPPUNIT_ASSERT(set_sync_is_equal_to_hash(&remoteprocess,localroot));

	// TRANSMITTED DATA TEST
	CPPUNIT_ASSERT(set_sync_received_bytes(&localprocess) == set_sync_sent_bytes(&remoteprocess));
	CPPUNIT_ASSERT(set_sync_received_bytes(&remoteprocess) == set_sync_sent_bytes(&localprocess));

	CPPUNIT_ASSERT(set_sync_free_handle(&localprocess)==0);
	CPPUNIT_ASSERT(set_sync_free_handle(&remoteprocess)==0);
	CPPUNIT_ASSERT(set_free(&localset)==0);
	CPPUNIT_ASSERT(set_free(&remoteset)==0);
}

void SetTest::testIterator() {
	setsync::Set set(config);
	for (unsigned int i = 1; i <= 100; i++) {
		stringstream ss;
		ss << "bla" << i;
		set.insert(ss.str());
	}

	Set::iterator begin = set.begin();
	Set::iterator end = set.end();
	Set::iterator iter = begin;
	size_t i = 1;
	while (iter != end) {
		i++;
		iter++;
	}
	CPPUNIT_ASSERT( i == set.getSize());
	i = 1;
	iter = begin;
	while (iter != end) {
		i++;
		++iter;
	}
	CPPUNIT_ASSERT( i == set.getSize());
	i = 1;
	iter = end;
	while (iter != begin) {
		i++;
		iter--;
	}
	CPPUNIT_ASSERT( i == set.getSize());
	i = 1;
	iter = end;
	while (iter != begin) {
		i++;
		--iter;
	}
	CPPUNIT_ASSERT( i == set.getSize());
}

void SetTest::setUp() {
	this->dir = new utils::FileSystem::TemporaryDirectory("temp_");
	this->config.setPath(dir->getPath());
}
void SetTest::tearDown() {
	delete this->dir;
}
}
