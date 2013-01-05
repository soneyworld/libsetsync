/*
 * SetSync.cpp
 *
 *      Author: Till Lorentzen
 */

#include "SetSync.h"
#include <setsync/set.h>

namespace evaluation {

SetSync::SetSync(const SET_CONFIG config, const size_t initA,
		const size_t initB, const size_t sameElements, const SyncType type,
		const string salt, const size_t maximumBufferSize) :
	initA_(initA), initB_(initB), initSameElements_(sameElements),
			bufferSize_(maximumBufferSize), type_(type), initSalt_(salt),
			configA_(config), configB_(config), A_(configA_), B_(configB_) {
	if (sameElements > initA || sameElements > initB) {
		throw "illegal arguments";
	}
	if (bufferSize_ < A_.getHashFunction().getHashSize() * 2) {
		throw "illegal argument: Buffer is too small";
	}
	switch (config.storage) {
	case LEVELDB:
		dbType_ = "LevelDB";
		break;
	case BERKELEY_DB:
		dbType_ = "Berkeley DB";
		break;
	default:
		dbType_ = "unknown";
		break;
	}
	hashType_ = A_.getHashFunction().getName();
	uint64_t i;
	for (i = 0; i < sameElements; i++) {
		stringstream ss;
		ss << salt << i;
		A_.insert(ss.str());
		B_.insert(ss.str());
	}
	uint64_t diffA = initA_ - initSameElements_;
	uint64_t j;
	for (j = 0; j < diffA; j++) {
		stringstream ss;
		ss << salt << i;
		A_.insert(ss.str());
		i++;
	}
	uint64_t diffB = initB_ - initSameElements_;
	for (j = 0; j < diffB; j++) {
		stringstream ss;
		ss << salt << i;
		B_.insert(ss.str());
		i++;
	}
	diffSize_ = diffA + diffB;
}

SetSync::~SetSync() {

}

void SetSync::runLooseSync(setsync::SynchronizationProcess * processA,
		setsync::SynchronizationProcess * processB) {
	unsigned char buffer[bufferSize_];
	std::size_t sending;
	setsync::ListDiffHandler handlerA;
	setsync::ListDiffHandler handlerB;
	while (processA->isBloomFilterOutputAvail()
			|| processB->isBloomFilterOutputAvail()) {
		if (processA->isBloomFilterOutputAvail()) {
			watchA.start();
			sending = processA->readSomeBloomFilter(buffer, bufferSize_);
			watchA.stop();
			watchB.start();
			processB->diffBloomFilter(buffer, sending, handlerB);
			watchB.stop();
			for (size_t i = 0; i < handlerB.size(); i++) {
				A_.insert(handlerB[i].first);
			}
			diffSize_ -= handlerB.size();
			handlerB.clear();
			printLine(processA, processB, "bloomA->B");
		}
		if (processB->isBloomFilterOutputAvail()) {
			watchB.start();
			sending = processB->readSomeBloomFilter(buffer, bufferSize_);
			watchB.stop();
			watchA.start();
			processA->diffBloomFilter(buffer, sending, handlerA);
			watchA.stop();
			for (size_t i = 0; i < handlerA.size(); i++) {
				B_.insert(handlerA[i].first);
			}
			diffSize_ -= handlerA.size();
			handlerA.clear();
			printLine(processA, processB, "bloomB->A");
		}

	}
	printLine(processA, processB, "bloom");
}

void SetSync::runStrictSync(setsync::SynchronizationProcess * processA,
		setsync::SynchronizationProcess * processB) {
	unsigned char buffer[bufferSize_];
	size_t subtriesize;
	std::size_t numberOfAcks;
	std::size_t acksize;
	setsync::ListDiffHandler handlerA;
	setsync::ListDiffHandler handlerB;
	while (!processA->done() && !processB->done()) {
		if (processA->isSubtrieOutputAvailable()) {
			watchA.start();
			subtriesize = processA->getSubTrie(buffer, bufferSize_);
			watchA.stop();
			watchB.start();
			acksize = processB->processSubTrie(buffer, subtriesize);
			watchB.stop();
			printLine(processA, processB, "trieSubA->B");
		}
		if (processA->isAckOutputAvailable()) {
			watchA.start();
			acksize = processA->readSomeTrieAcks(buffer, bufferSize_,
					&numberOfAcks);
			watchA.stop();
			watchB.start();
			processB->processAcks(buffer, acksize, numberOfAcks, handlerB);
			watchB.stop();
			for (std::size_t i = 0; i < handlerB.size(); i++) {
				A_.insert(handlerB[i].first);
			}
			diffSize_ -= handlerB.size();
			handlerB.clear();
			printLine(processA, processB, "trieAckA->B");
		}
		if (processB->isSubtrieOutputAvailable()) {
			watchB.start();
			subtriesize = processB->getSubTrie(buffer, bufferSize_);
			watchB.stop();
			watchA.start();
			acksize = processA->processSubTrie(buffer, subtriesize);
			watchA.stop();
			printLine(processA, processB, "trieSubB->A");
		}
		if (processB->isAckOutputAvailable()) {
			watchB.start();
			acksize = processB->readSomeTrieAcks(buffer, bufferSize_,
					&numberOfAcks);
			watchB.stop();
			watchA.start();
			processA->processAcks(buffer, acksize, numberOfAcks, handlerA);
			watchA.stop();
			for (std::size_t i = 0; i < handlerA.size(); i++) {
				B_.insert(handlerA[i].first);
			}
			diffSize_ -= handlerA.size();
			handlerA.clear();
			printLine(processA, processB, "trieAckB->A");
		}
	}
}

void SetSync::printLine(setsync::SynchronizationProcess * processA,
		setsync::SynchronizationProcess * processB, const string& phase) const {

	cout << A_.getSize() << ",";
	cout << B_.getSize() << ",";
	cout << diffSize_ << ",";
	cout << processA->getSentBytes() << ",";
	cout << processB->getSentBytes() << ",";
	cout << phase << ",";
	cout << watchA.getDuration() << ",";
	cout << watchB.getDuration() << ",";
	cout << watchA.getLastDuration() << ",";
	cout << watchB.getLastDuration() << ",";
	cout << watchA.getCPUDuration() << ",";
	cout << watchB.getCPUDuration() << ",";
	cout << watchA.getLastCPUDuration() << ",";
	cout << watchB.getLastCPUDuration() << endl;
}

void SetSync::run() {
	string t;
	switch (type_) {
	case LOOSE:
		t = "bloomfilter";
		break;
	case STRICT:
		t = "trie";
		break;
	case BOTH:
		t = "bloomfilter+trie";
		break;
	}
	cout << "running SetSync Test:" << endl;
	cout << "\tDB=" << dbType_ << endl;
	cout << "\tSyncType=" << t << endl;
	cout << "\tHashFunction=" << hashType_ << endl;
	cout << "\tBuffersize=" << bufferSize_ << endl;
	cout << "\tMinimalBuffersize=" << A_.getMinSyncBuffer() << endl;
	cout << "\tMinimalListTransferA->B=" << A_.getSize()
			* A_.getHashFunction().getHashSize() + (B_.getSize() + 7) / 8
			<< endl;
	cout << "\tMinimalListTransferB->A=" << B_.getSize()
			* B_.getHashFunction().getHashSize() + (A_.getSize() + 7) / 8
			<< endl;
	cout << endl;
	cout
			<< "sizeofA,sizeofB,diffSize,sentBytesA,sentBytesB,syncPhase,syncDurationA,syncDurationB,syncLastDurationA,syncLastDurationB,syncCPU_A,syncCPU_B,syncLastCPU_A,syncLastCPU_B"
			<< endl;
	setsync::SynchronizationProcess * processA = A_.createSyncProcess();
	setsync::SynchronizationProcess * processB = B_.createSyncProcess();
	printLine(processA, processB, "start");
	if (type_ == LOOSE || type_ == BOTH) {
		runLooseSync(processA, processB);
	}
	if (type_ == STRICT || type_ == BOTH) {
		runStrictSync(processA, processB);
	}
}

}
