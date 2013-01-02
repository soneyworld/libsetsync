/*
 * Synchronization.cpp
 *
 *      Author: Till Lorentzen
 */

#include "Synchronization.h"

namespace setsync {

namespace sync {

AbstractSyncProcessPart::AbstractSyncProcessPart() :
	optimalPacketSize_(4096) {
}

AbstractSyncProcessPart::~AbstractSyncProcessPart() {
}

void AbstractSyncProcessPart::setOptimalPacketSize(const std::size_t s) {
	this->optimalPacketSize_ = s;
}

std::size_t AbstractSyncProcessPart::getOptimalPacketSize() const {
	return this->optimalPacketSize_;
}

HashSyncProcessPart::HashSyncProcessPart(const utils::CryptoHash& hashFunction,
		const unsigned char * localHashDigit) :
	hashsize_(hashFunction.getHashSize()), inPos_(0), outPos_(0) {
	this->inHash_ = (unsigned char *) malloc(hashsize_);
	this->outHash_ = (unsigned char *) malloc(hashsize_);
	memcpy(this->outHash_, localHashDigit, hashsize_);
}

HashSyncProcessPart::~HashSyncProcessPart() {
	free(this->inHash_);
	free(this->outHash_);
}

bool HashSyncProcessPart::pendingOutput() const {
	return this->outPos_ < hashsize_;
}

bool HashSyncProcessPart::awaitingInput() const {
	return this->inPos_ < hashsize_;
}

bool HashSyncProcessPart::done() const {
	if (awaitingInput())
		return false;
	if (pendingOutput())
		return false;
	return true;
}

std::size_t HashSyncProcessPart::processInput(void * input,
		const std::size_t length, AbstractDiffHandler& diffhandler) {
	std::size_t min = std::min(hashsize_ - inPos_, length);
	memcpy(this->inHash_ + inPos_, input, min);
	inPos_ += min;
	return min;
}
std::size_t HashSyncProcessPart::writeOutput(void * outbuf,
		const std::size_t maxlength) {
	std::size_t min = std::min(hashsize_ - outPos_, maxlength);
	memcpy(outbuf, this->outHash_ + outPos_, min);
	outPos_ += min;
	return min;
}

bool HashSyncProcessPart::isEqual() const {
	if (inPos_ == hashsize_) {
		return memcmp(this->inHash_, this->outHash_, hashsize_) == 0;
	} else {
		return false;
	}
}

std::size_t HashSyncProcessPart::getRemainigOutputPacketSize() const {
	return this->hashsize_ - this->outPos_;
}

bool HashSyncProcessPart::parsingOfLastPacketDone() const {
	return !this->awaitingInput();
}

}
}
