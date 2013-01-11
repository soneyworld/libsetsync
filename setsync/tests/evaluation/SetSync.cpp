/*
 * SetSync.cpp
 *
 *      Author: Till Lorentzen
 */

#include "SetSync.h"
#include <setsync/set.h>
#include <sstream>
#include <cstdlib>
#include <setsync/crypto/CryptoHash.h>
#include <setsync/utils/OutputFunctions.h>

namespace evaluation {

SetSync::SetSync(const SET_CONFIG config, const size_t initA,
		const size_t initB, const size_t sameElements, const SyncType type,
		const string salt, const size_t maximumBufferSize, const bool printDot) :
	initA_(initA), initB_(initB), initSameElements_(sameElements),
			bufferSize_(maximumBufferSize), type_(type), initSalt_(salt),
			configA_(config), configB_(config), A_(configA_), B_(configB_),
			printDot_(printDot) {
	if (sameElements > initA || sameElements > initB) {
		if (sameElements > initA) {
			cout << "Illegal argument: A is smaller as equal elements" << endl;
		} else {
			cout << "illegal argument: B is smaller as equal elements" << endl;
		}
		exit(-1);
	}
	if (bufferSize_ < A_.getHashFunction().getHashSize() * 2) {
		cout << "illegal argument: Buffer is too small" << endl;
		exit(-1);
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
	size_t i;
	for (i = 0; i < initSameElements_; i++) {
		stringstream ss;
		ss << salt << "_" << i;
		insertWatchA.start();
		A_.insert(ss.str());
		insertWatchA.stop();
		insertWatchB.start();
		B_.insert(ss.str());
		insertWatchB.stop();
	}
	size_t diffA = initA_ - initSameElements_;
	size_t j;
	for (j = 0; j < diffA; j++) {
		stringstream ss;
		ss << salt << "_" << i;
		insertWatchA.start();
		A_.insert(ss.str());
		insertWatchA.stop();
		i++;
	}
	size_t diffB = initB_ - initSameElements_;
	for (j = 0; j < diffB; j++) {
		stringstream ss;
		ss << salt << "_" << i;
		insertWatchB.start();
		B_.insert(ss.str());
		insertWatchB.stop();
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
			sending = processA->readNextBloomFilterChunk(buffer, bufferSize_);
			watchA.stop();
			watchB.start();
			processB->processBloomFilterChunk(buffer, sending, handlerB);
			watchB.stop();
			for (size_t i = 0; i < handlerB.size(); i++) {
				insertWatchA.start();
				if (A_.insert(handlerB[i].first))
					diffSize_--;
				insertWatchA.stop();
			}
			handlerB.clear();
			printLine(processA, processB, "bloomA->B");
		}
		if (processB->isBloomFilterOutputAvail()) {
			watchB.start();
			sending = processB->readNextBloomFilterChunk(buffer, bufferSize_);
			watchB.stop();
			watchA.start();
			processA->processBloomFilterChunk(buffer, sending, handlerA);
			watchA.stop();
			for (size_t i = 0; i < handlerA.size(); i++) {
				insertWatchB.start();
				if (B_.insert(handlerA[i].first)) {
					diffSize_--;
				}
				insertWatchB.stop();
			}
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
		while (processA->isAckOutputAvailable()) {
			watchA.start();
			acksize = processA->readSomeTrieAcks(buffer, bufferSize_,
					&numberOfAcks);
			watchA.stop();
			watchB.start();
			processB->processAcks(buffer, acksize, numberOfAcks, handlerB);
			watchB.stop();
			for (std::size_t i = 0; i < handlerB.size(); i++) {
				insertWatchA.start();
				if (A_.insert(handlerB[i].first)) {
					diffSize_--;
				}
				insertWatchA.stop();
			}
			handlerB.clear();
			printLine(processA, processB, "trieAckA->B");
		}
		while (processB->isAckOutputAvailable()) {
			watchB.start();
			acksize = processB->readSomeTrieAcks(buffer, bufferSize_,
					&numberOfAcks);
			watchB.stop();
			watchA.start();
			processA->processAcks(buffer, acksize, numberOfAcks, handlerA);
			watchA.stop();
			for (std::size_t i = 0; i < handlerA.size(); i++) {
				insertWatchB.start();
				if (B_.insert(handlerA[i].first))
					diffSize_--;
				insertWatchB.stop();
			}
			handlerA.clear();
			printLine(processA, processB, "trieAckB->A");
		}
		while (processB->isSubtrieOutputAvailable()) {
			watchB.start();
			subtriesize = processB->getSubTrie(buffer, bufferSize_);
			watchB.stop();
			watchA.start();
			acksize = processA->processSubTrie(buffer, subtriesize);
			watchA.stop();
			printLine(processA, processB, "trieSubB->A");
		}
		while (processA->isSubtrieOutputAvailable()) {
			watchA.start();
			subtriesize = processA->getSubTrie(buffer, bufferSize_);
			watchA.stop();
			watchB.start();
			acksize = processB->processSubTrie(buffer, subtriesize);
			watchB.stop();
			printLine(processA, processB, "trieSubA->B");
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
	if (!(type_ == STRICT)) {
		cout << "\tBloomFilterSize=" << A_.getBFSize() << endl;
		cout << "\tBloomFilterFunctionCount=" << A_.getBFFunctionCount()
				<< endl;
		cout << "\tBloomFilterFalsePositiveRate="
				<< configA_.getBloomFilter().falsePositiveRate << endl;
	}
	cout << endl;
	cout << "\tMinimalListTransferA->B=" << A_.getSize()
			* A_.getHashFunction().getHashSize() << endl;
	cout << "\tMinimalListTransferB->A=" << B_.getSize()
			* B_.getHashFunction().getHashSize() << endl;
	cout << endl;
	cout << "======== CSV ========" << endl;
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
	if ((A_ != B_)) {
		printLine(processA, processB, "finishedWithError");
		if (printDot_) {
			cout << "digraph error {" << endl;
			cout << A_.getTrieToDot("A");
			cout << B_.getTrieToDot("B");
			size_t diffA = initA_ - initSameElements_;
			size_t j;
			string color = "red";
			size_t i = initSameElements_;
			const setsync::utils::CryptoHash& hash = A_.getHashFunction();
			unsigned char buffer[hash.getHashSize()];
			for (j = 0; j < diffA; j++) {
				stringstream ss;
				ss << initSalt_ << "_" << i;
				hash(buffer, ss.str());
				if (!B_.find(buffer)) {

					cout << "A" << setsync::utils::OutputFunctions::CryptoHashtoString(
							buffer, hash.getHashSize())
							<< " [style=filled color=\"" << color << "\"];"
							<< endl;
				}
				i++;
			}
			size_t diffB = initB_ - initSameElements_;
			for (j = 0; j < diffB; j++) {
				stringstream ss;
				ss << initSalt_ << "_" << i;
				hash(buffer, ss.str());
				if (!A_.find(buffer)) {
					cout << "B" << setsync::utils::OutputFunctions::CryptoHashtoString(
							buffer, hash.getHashSize())
							<< " [style=filled color=\"" << color << "\"];"
							<< endl;
				}
				i++;
			}

			cout << "}" << endl;
		}
	} else {
		printLine(processA, processB, "finished");
	}
	cout << endl;
	cout << "Ainserts(secs),Binserts(secs),CPUAinserts(secs),CPUBinserts(secs)"
			<< endl;
	cout << insertWatchA.getDuration() << "," << insertWatchB.getDuration()
			<< "," << insertWatchA.getCPUDuration() << ","
			<< insertWatchB.getCPUDuration() << endl;
	delete processA;
	delete processB;
}

}
