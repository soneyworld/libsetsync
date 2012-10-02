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
#include "setsync/sha1.h"
#include <sstream>
#include <typeinfo>
#include <algorithm>
namespace bloom {

uint64_t AbstractBloomFilter::numberOfElements() const {
	return this->itemCount_;
}

std::size_t AbstractBloomFilter::size() const {
	return (this->filterSize_ + (BYTESIZE - 1)) / BYTESIZE;
}

uint64_t AbstractBloomFilter::exactBitSize() const {
	return this->filterSize_;
}

void AbstractBloomFilter::add(const std::string& key) {
	unsigned char c[SHA_DIGEST_LENGTH];
	SHA1((unsigned char*) key.c_str(), key.size(), c);
	add(c);
}

void AbstractBloomFilter::add(const char *string) {
	unsigned char c[SHA_DIGEST_LENGTH];
	SHA1((unsigned char*) string, strlen(string), c);
	add(c);
}

bool AbstractBloomFilter::contains(const std::string& key) const {
	unsigned char c[SHA_DIGEST_LENGTH];
	SHA1((unsigned char*) key.c_str(), key.size(), c);
	return contains(c);
}

bool AbstractBloomFilter::contains(const char * string) const {
	unsigned char c[SHA_DIGEST_LENGTH];
	SHA1((unsigned char*) string, strlen(string), c);
	return contains(c);
}

const unsigned char AbstractBloomFilter::bit_mask[BYTESIZE] = { 0x01, //00000001
		0x02, //00000010
		0x04, //00000100
		0x08, //00001000
		0x10, //00010000
		0x20, //00100000
		0x40, //01000000
		0x80 //10000000
		};

BloomFilter::BloomFilter(const BloomFilter& filter) {
	this->itemCount_ = filter.itemCount_;
	this->maxElements_ = filter.maxElements_;
	this->filterSize_ = filter.filterSize_;
	this->hardMaximum_ = filter.hardMaximum_;
	this->functionCount_ = filter.functionCount_;
	this->hashsize_ = filter.hashsize_;
	this->bitArray_ = (unsigned char *) malloc(
			(this->filterSize_ + (BYTESIZE - 1)) / BYTESIZE);
	memcpy(this->bitArray_, filter.bitArray_,
			(this->filterSize_ + (BYTESIZE - 1)) / BYTESIZE);
}

BloomFilter::BloomFilter(const uint64_t maxNumberOfElements,
		const bool hardMaximum, const float falsePositiveRate,
		const std::size_t hashsize) {
	this->hashsize_ = hashsize;
	init(falsePositiveRate, hardMaximum, maxNumberOfElements);
	this->hashFunction_ = new SaltedHashFunction(this->functionCount_);
}

BloomFilter::BloomFilter(const std::string hashFunction,
		const uint64_t maxNumberOfElements, const bool hardMaximum,
		const float falsePositiveRate, const std::size_t hashsize) {
	this->hashsize_ = hashsize;
	init(falsePositiveRate, hardMaximum, maxNumberOfElements);
	this->hashFunction_
			= HashFunctionFactory::getInstance().createHashFunction(
					hashFunction);
}

BloomFilter::~BloomFilter() {
	if (this->bitArray_ != NULL)
		free(this->bitArray_);
	delete this->hashFunction_;
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

AbstractBloomFilter& BloomFilter::operator &=(const AbstractBloomFilter& filter) {
	/* intersection */
	const BloomFilter& filter_ = dynamic_cast<const BloomFilter&> (filter);
	if (this->filterSize_ == filter_.filterSize_) {
		for (std::size_t i = 0; i < ((this->filterSize_ + (BYTESIZE - 1))
				/ BYTESIZE); ++i) {
			this->bitArray_[i] &= filter_.bitArray_[i];
		}
	}
	return *this;
}

AbstractBloomFilter& BloomFilter::operator |=(const AbstractBloomFilter& filter) {
	/* union */
	const BloomFilter& filter_ = dynamic_cast<const BloomFilter&> (filter);
	if (this->filterSize_ == filter_.filterSize_) {
		for (std::size_t i = 0; i < ((this->filterSize_ + (BYTESIZE - 1))
				/ BYTESIZE); ++i) {
			this->bitArray_[i] |= filter_.bitArray_[i];
		}
	}
	return *this;
}

AbstractBloomFilter& BloomFilter::operator ^=(const AbstractBloomFilter& filter) {
	/* difference */
	const BloomFilter& filter_ = dynamic_cast<const BloomFilter&> (filter);
	if (this->filterSize_ == filter_.filterSize_) {
		for (std::size_t i = 0; i < ((this->filterSize_ + (BYTESIZE - 1))
				/ BYTESIZE); ++i) {
			this->bitArray_[i] ^= filter_.bitArray_[i];
		}
	}
	return *this;
}

bool BloomFilter::operator ==(const AbstractBloomFilter& filter) const {
	/* equals */
	try {
		const BloomFilter& filter_ = dynamic_cast<const BloomFilter&> (filter);
		if (this->filterSize_ != filter_.filterSize_)
			return false;
		int n = memcmp(this->bitArray_, filter_.bitArray_,
				this->filterSize_ / BYTESIZE);
		if (n != 0)
			return false;
		int remainder = this->filterSize_ % BYTESIZE;
		if (remainder == 0)
			return true;
		unsigned char c1 = this->bitArray_[this->filterSize_ / BYTESIZE];
		unsigned char c2 = filter_.bitArray_[this->filterSize_ / BYTESIZE];
		c1 >> BYTESIZE - remainder;
		c2 >> BYTESIZE - remainder;
		return c1 == c2;
	} catch (const std::bad_cast& e) {
		return false;
	}
}

bool BloomFilter::operator !=(const AbstractBloomFilter& filter) const {
	/* diffs */
	try {
		const BloomFilter& filter_ = dynamic_cast<const BloomFilter&> (filter);

		if (this->filterSize_ != filter_.filterSize_)
			return true;
		int n = memcmp(this->bitArray_, filter_.bitArray_,
				this->filterSize_ / BYTESIZE);
		if (n != 0)
			return true;
		int remainder = this->filterSize_ % BYTESIZE;
		if (remainder == 0)
			return false;
		unsigned char c1 = this->bitArray_[this->filterSize_ / BYTESIZE];
		unsigned char c2 = filter_.bitArray_[this->filterSize_ / BYTESIZE];
		c1 >> BYTESIZE - remainder;
		c2 >> BYTESIZE - remainder;
		return c1 != c2;
	} catch (const std::bad_cast& e) {
		return false;
	}
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
		uint64_t pos = this->hashFunction_->hash(key, this->hashsize_, i);
		compute_indices(pos, bit_index, bit);
		this->bitArray_[bit_index / BYTESIZE] |= bit_mask[bit];
	}
	if (this->itemCount_ < std::numeric_limits<uint64_t>::max())
		this->itemCount_++;
}

void BloomFilter::addAll(const unsigned char *keys, const std::size_t count) {
	if (this->hardMaximum_ && this->itemCount_ + count > maxElements_)
		throw "Maximum of Elements reached, adding failed";
	uint64_t hashes[count * this->functionCount_];
	for (int i = 0; i < count; i++) {
		for (int j = 0; j < this->functionCount_; j++) {
			hashes[i * this->functionCount_ + j] = this->hashFunction_->hash(
					keys + (this->hashsize_ * i), this->hashsize_, j);
		}
	}
	std::sort(hashes, hashes + (count * this->functionCount_));
	std::size_t bit_index = 0;
	std::size_t bit = 0;
	for (int i = 0; i < count * this->functionCount_; i++) {
		compute_indices(hashes[i], bit_index, bit);
		this->bitArray_[bit_index / BYTESIZE] |= bit_mask[bit];
	}
	if (this->itemCount_ + count < std::numeric_limits<uint64_t>::max()) {
		this->itemCount_ += count;
	} else {
		this->itemCount_ = std::numeric_limits<uint64_t>::max();
	}
}

void BloomFilter::load(std::istream &in, const uint64_t numberOfElements) {
	if (this->itemCount_ == 0) {
		// Adding exception, if end of input stream reached
		in.exceptions(in.exceptions() | std::istream::eofbit);
		// read data as a block:
		in.read((char*) this->bitArray_, (this->filterSize_ + 7) / BYTESIZE);
		this->itemCount_ = numberOfElements;
	} else {
		throw "This bloom filter already has got elements";
	}
}

uint64_t BloomFilter::save(std::ostream &out) {
	out.write((char*) this->bitArray_, (this->filterSize_ + 7) / BYTESIZE);
	return this->itemCount_;
}

bool BloomFilter::contains(const unsigned char *key) const {
	std::size_t bit_index = 0;
	std::size_t bit = 0;

	for (int i = 0; i < this->functionCount_; i++) {
		uint64_t pos = this->hashFunction_->hash(key, SHA_DIGEST_LENGTH, i);
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

std::string BloomFilter::toString() {
	std::stringstream ss;
	for (int i = 0; i < (this->filterSize_ + 7) / BYTESIZE; i++) {
		unsigned char byte = this->bitArray_[i];
		for (int j = 7; j >= 0; j--) {
			if (byte & (1 << j))
				ss << "1";
			else
				ss << "0";
		}
	}
	return ss.str();
}

}
