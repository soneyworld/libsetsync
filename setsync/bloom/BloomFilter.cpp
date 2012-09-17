/*
 * BloomFilter.cpp
 *
 *      Author: Till Lorentzen
 *
 * This bloom filter implementation is inspired by
 * Open Bloom Filter (http://www.partow.net) written by Arash Partow.
 *
 * And also by ibrcommon written by Johannes Morgenroth <morgenroth@ibr.cs.tu-bs.de>
 */

#include "BloomFilter.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <limits>

namespace bloom {

const unsigned char BloomFilter::bit_mask[BYTESIZE] = { 0x01, //00000001
		0x02, //00000010
		0x04, //00000100
		0x08, //00001000
		0x10, //00010000
		0x20, //00100000
		0x40, //01000000
		0x80 //10000000
		};
BloomFilter::BloomFilter(const BloomFilter& filter) :
	filterSize_(filter.filterSize_), itemCount_(filter.itemCount_),
			maxElements_(filter.maxElements_),
			hardMaximum_(filter.hardMaximum_),
			functionCount_(filter.functionCount_) {
	this->bitArray_ = (unsigned char *) malloc(
			(this->filterSize_ + (BYTESIZE - 1)) / BYTESIZE);
	memcpy(this->bitArray_, filter.bitArray_,
			(this->filterSize_ + (BYTESIZE - 1)) / BYTESIZE);
}

BloomFilter::BloomFilter(const uint64_t maxNumberOfElements,
		const bool hardMaximum, const float falsePositiveRate,
		const std::size_t hashsize) {
	init(falsePositiveRate, hardMaximum, maxNumberOfElements);
	this->hashFunction_ = new SaltedHashFunction(this->functionCount_);
}

BloomFilter::BloomFilter(HashFunction* hashFunction,
		const uint64_t maxNumberOfElements, const bool hardMaximum,
		const float falsePositiveRate, const std::size_t hashsize) {
	init(falsePositiveRate, hardMaximum, maxNumberOfElements);
	this->hashFunction_ = hashFunction;
}

BloomFilter::~BloomFilter() {
	if (this->bitArray_ != NULL)
		free(this->bitArray_);
}

std::size_t BloomFilter::size() const {
	return (this->filterSize_ + (BYTESIZE - 1)) / BYTESIZE;
}

uint64_t BloomFilter::exactBitSize() const {
	return this->filterSize_;
}

BloomFilter& BloomFilter::operator=(const BloomFilter& filter) {
	this->filterSize_ = filter.filterSize_;
	this->hardMaximum_ = filter.hardMaximum_;
	this->functionCount_ = filter.functionCount_;
	this->itemCount_ = filter.itemCount_;
	if (this->bitArray_ != NULL)
		free(this->bitArray_);
	this->bitArray_ = (unsigned char *) malloc(
			(this->filterSize_ + (BYTESIZE - 1)) / BYTESIZE);
	if (this->bitArray_ == NULL)
		throw "MALLOC FAILD!";
	memcpy(this->bitArray_, filter.bitArray_,
			(this->filterSize_ + (BYTESIZE - 1)) / BYTESIZE);
	return *this;
}

BloomFilter& BloomFilter::operator &=(const BloomFilter& filter) {
	/* intersection */
	if (this->filterSize_ == filter.filterSize_) {
		for (std::size_t i = 0; i < ((this->filterSize_ + (BYTESIZE - 1))
				/ BYTESIZE); ++i) {
			this->bitArray_[i] &= filter.bitArray_[i];
		}
	}
	return *this;
}

BloomFilter& BloomFilter::operator |=(const BloomFilter& filter) {
	/* union */
	if (this->filterSize_ == filter.filterSize_) {
		for (std::size_t i = 0; i < ((this->filterSize_ + (BYTESIZE - 1))
				/ BYTESIZE); ++i) {
			this->bitArray_[i] |= filter.bitArray_[i];
		}
	}
	return *this;
}

BloomFilter& BloomFilter::operator ^=(const BloomFilter& filter) {
	/* difference */
	if (this->filterSize_ == filter.filterSize_) {
		for (std::size_t i = 0; i < ((this->filterSize_ + (BYTESIZE - 1))
				/ BYTESIZE); ++i) {
			this->bitArray_[i] ^= filter.bitArray_[i];
		}
	}
	return *this;
}

bool BloomFilter::operator ==(const BloomFilter& filter) const {
	/* equals */
	if (this->filterSize_ != filter.filterSize_)
		return false;
	int n = memcmp(this->bitArray_, filter.bitArray_,
			this->filterSize_ / BYTESIZE);
	if (n != 0)
		return false;
	int remainder = this->filterSize_ % BYTESIZE;
	if (remainder == 0)
		return true;
	unsigned char c1 = this->bitArray_[this->filterSize_ / BYTESIZE];
	unsigned char c2 = filter.bitArray_[this->filterSize_ / BYTESIZE];
	c1 >> BYTESIZE - remainder;
	c2 >> BYTESIZE - remainder;
	return c1 == c2;
}

bool BloomFilter::operator !=(const BloomFilter& filter) const {
	/* diffs */
	if (this->filterSize_ != filter.filterSize_)
		return true;
	int n = memcmp(this->bitArray_, filter.bitArray_,
			this->filterSize_ / BYTESIZE);
	if (n != 0)
		return true;
	int remainder = this->filterSize_ % BYTESIZE;
	if (remainder == 0)
		return false;
	unsigned char c1 = this->bitArray_[this->filterSize_ / BYTESIZE];
	unsigned char c2 = filter.bitArray_[this->filterSize_ / BYTESIZE];
	c1 >> BYTESIZE - remainder;
	c2 >> BYTESIZE - remainder;
	return c1 != c2;
}

bool BloomFilter::operator!() const {
	return (0 == this->filterSize_);
}

void BloomFilter::clear() {
	memset(this->bitArray_, 0x00,
			(this->filterSize_ + (BYTESIZE - 1)) / BYTESIZE);
	this->itemCount_ = 0;
}

void BloomFilter::add(const unsigned char *key) {
	if (this->hardMaximum_ && this->itemCount_ >= maxElements_)
		throw "Maximum of Elements reached, adding failed";
	std::size_t bit_index = 0;
	std::size_t bit = 0;
	for (int i = 0; i < this->functionCount_; i++) {
		uint64_t pos = this->hashFunction_->hash(key, 20, i);
		compute_indices(pos, bit_index, bit);
		this->bitArray_[bit_index / BYTESIZE] |= bit_mask[bit];
	}
	if (this->itemCount_ < std::numeric_limits<uint64_t>::max())
		this->itemCount_++;
}

void BloomFilter::load(std::istream &in) {
	//TODO
	throw "MUST BE IMPLEMENTED";
}

bool BloomFilter::contains(const unsigned char *key) const {
	std::size_t bit_index = 0;
	std::size_t bit = 0;

	for (int i = 0; i < this->functionCount_; i++) {
		uint64_t pos = this->hashFunction_->hash(key, 20, i);
		compute_indices(pos, bit_index, bit);
		if ((this->bitArray_[bit_index / BYTESIZE] & bit_mask[bit])
				!= bit_mask[bit]) {
			return false;
		}
	}
	return true;
}

std::size_t BloomFilter::containsAll(const unsigned char *keys,
		const std::size_t count) const {
	std::size_t i;
	for (i = 0; i < count; i++) {
		if (!this->contains(keys + (this->hashsize_ * i))) {
			return i;
		}
	}
	return count;
}
void BloomFilter::compute_indices(const uint64_t hash, std::size_t& bit_index,
		std::size_t& bit) const {
	bit_index = hash % this->filterSize_;
	bit = bit_index % BYTESIZE;
}

void BloomFilter::init(const float falsePositiveRate, const bool hardMaximum,
		const uint64_t numberOfElements) {
	this->bitArray_ = NULL;
	if (falsePositiveRate >= 1 || falsePositiveRate <= 0)
		throw "";

	this->hardMaximum_ = hardMaximum;
	if (this->hardMaximum_)
		this->maxElements_ = numberOfElements;

	this->filterSize_ = ceil(
			(numberOfElements * log(falsePositiveRate)) / log(
					1.0 / (pow(2.0, log(2.0)))));
	// Calculating the number of hash functions necessary to reach the given
	this->functionCount_ = round(
			log(2.0) * this->filterSize_ / numberOfElements);
	// Minimal count of hash functions has to be 1
	if (this->functionCount_ < 1)
		this->functionCount_ = 1;

	this->bitArray_ = (unsigned char *) malloc(
			(this->filterSize_ + (BYTESIZE - 1)) / BYTESIZE);
	if (this->bitArray_ == NULL) {
		throw "MALLOC failed!";
	}
	memset(this->bitArray_, 0x00,
			(this->filterSize_ + (BYTESIZE - 1)) / BYTESIZE);
	this->itemCount_ = 0;
}

}
