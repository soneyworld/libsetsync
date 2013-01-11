/*
 * FSBloomFilter.cpp
 *
 *      Author: Till Lorentzen
 */

#include "FSBloomFilter.h"
#include <limits>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <stdio.h>
#include <typeinfo>
#include <algorithm>
#include <sstream>
#include "DoubleHashingScheme.h"
#include <stdexcept>
#include <setsync/utils/OutputFunctions.h>
#include <unistd.h>

#ifndef BYTESIZE
#define BYTESIZE 8
#endif

#define _unused(x) ((void)x)
namespace setsync {
namespace bloom {

FSBloomFilter::FSBloomFilter(const crypto::CryptoHash& hash, const char * file,
		const uint64_t maxNumberOfElements, const bool hardMaximum,
		const float falsePositiveRate) :
	AbstractBloomFilter(hash), filehandler_(NULL) {
	if (file != NULL) {
		// If file at the file path exists, just open it
		filehandler_ = fopen(file, "r+");
		// If file doesn't exist, try to create a new one
		if (filehandler_ == NULL) {
			filehandler_ = fopen(file, "w+");
		}
		// If creating a new one also failed, print out message and try to create temp file later
		if (filehandler_ == NULL) {
			std::cerr << "Fail on opening given file: " << file << std::endl;
			std::cerr << "Trying to open and use temporary file instead!"
					<< std::endl;
		}
	}
	init(falsePositiveRate, hardMaximum, maxNumberOfElements);
	this->hashFunction_ = new DoubleHashingScheme(
			this->cryptoHashFunction_.getHashSize());
}

void FSBloomFilter::init(const float falsePositiveRate, const bool hardMaximum,
		const uint64_t numberOfElements) {
	this->bitArray_ = NULL;
	if (falsePositiveRate >= 1 || falsePositiveRate <= 0) {
		std::stringstream ss;
		ss << "Wrong false positive rate: "<<falsePositiveRate;
		std::cout << ss.str()<< std::endl;
		throw std::runtime_error(ss.str());
	}

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
	this->mmapLength_ = (this->filterSize_ + (BYTESIZE - 1)) / BYTESIZE;
	if (this->filehandler_ == NULL)
		this->filehandler_ = tmpfile();
	if (this->filehandler_ == NULL) {
		std::cout << "TEMP File fail!" << std::endl;
		throw std::runtime_error("TEMP File fail!");
	}
	int fd = fileno(this->filehandler_);
	lseek(fd, this->mmapLength_ - 1, SEEK_SET);
	ssize_t filesize = write(fd, "", 1);
	if (filesize == -1) {
		std::cout << "writing to bloom filter file failed!" << std::endl;
		throw std::runtime_error("writing to bloom filter file failed!");
	}
	this->bitArray_ = (unsigned char *) mmap(NULL, this->mmapLength_,
			PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (this->bitArray_ == NULL) {
		std::cout << "MMAP failed!" << std::endl;
		throw std::runtime_error("MMAP failed!");
	}
	//	madvise(this->bitArray_, this->mmapLength_, MADV_SEQUENTIAL);
	memset(this->bitArray_, 0x00, this->mmapLength_);
	this->itemCount_ = 0;
}

FSBloomFilter::~FSBloomFilter() {
	int ret = munmap(this->bitArray_, this->mmapLength_);
#ifdef DEBUG
	if(ret==-1) {
		throw "THIS SHOULD NEVER EVER HAPPEN!!!! OTHERWISE, THIS CLASS IS WRONG!!!";
	}
#else
	_unused(ret);
#endif
	ret = fclose(this->filehandler_);
#ifdef DEBUG
	if(ret!=0) {
		throw "closing file failed, this shouldn't happen, but it is not important";
	}
#else
	_unused(ret);
#endif
	delete this->hashFunction_;
}

void FSBloomFilter::load(std::istream &in, const uint64_t numberOfElements) {
	if (this->itemCount_ == 0) {
		// Adding exception, if end of input stream reached
		in.exceptions(in.exceptions() | std::istream::eofbit);
		// read data as a block:
		in.read((char*) this->bitArray_, this->mmapLength_);
		this->itemCount_ = numberOfElements;
	} else {
		throw std::runtime_error("This bloom filter already has got elements");
	}
}
uint64_t FSBloomFilter::save(std::ostream &out) {
	out.write((char*) this->bitArray_, this->mmapLength_);
	return this->itemCount_;
}
void FSBloomFilter::clear() {
	memset(this->bitArray_, 0x00, this->mmapLength_);
	this->itemCount_ = 0;
}

void FSBloomFilter::add(const unsigned char *key) {
	if (this->hardMaximum_ && this->itemCount_ >= maxElements_)
		throw std::runtime_error("Maximum of Elements reached, adding failed");
	std::size_t bit_index = 0;
	std::size_t bit = 0;
	for (std::size_t i = 0; i < this->functionCount_; i++) {
		uint64_t pos = this->hashFunction_->operator ()(key,
				this->cryptoHashFunction_.getHashSize(), i) % this->filterSize_;
		compute_indices(pos, bit_index, bit);
		this->bitArray_[bit_index] |= bit_mask[bit];
	}
	if (this->itemCount_ < std::numeric_limits<uint64_t>::max())
		this->itemCount_++;
}

void FSBloomFilter::addAll(const unsigned char* keys, const std::size_t count) {
	if (this->hardMaximum_ && this->itemCount_ + count > maxElements_)
		throw std::runtime_error("Maximum of Elements reached, adding failed");
	uint64_t hashes[count * this->functionCount_];
	for (std::size_t i = 0; i < count; i++) {
		for (std::size_t j = 0; j < this->functionCount_; j++) {
			hashes[i * this->functionCount_ + j]
					= this->hashFunction_->operator ()(
							keys
									+ (this->cryptoHashFunction_.getHashSize()
											* i),
							this->cryptoHashFunction_.getHashSize(), j);
		}
	}
	std::sort(hashes, hashes + (count * this->functionCount_));
	std::size_t bit_index = 0;
	std::size_t bit = 0;
	for (std::size_t i = 0; i < count * this->functionCount_; i++) {
		compute_indices(hashes[i], bit_index, bit);
		this->bitArray_[bit_index] |= bit_mask[bit];
	}
	if (this->itemCount_ + count < std::numeric_limits<uint64_t>::max()) {
		this->itemCount_ += count;
	} else {
		this->itemCount_ = std::numeric_limits<uint64_t>::max();
	}
}

bool FSBloomFilter::contains(const unsigned char *key) const {
	std::size_t bit_index = 0;
	std::size_t bit = 0;

	for (std::size_t i = 0; i < this->functionCount_; i++) {
		uint64_t pos = this->hashFunction_->operator ()(key,
				this->cryptoHashFunction_.getHashSize(), i) % this->filterSize_;
		compute_indices(pos, bit_index, bit);
		if ((this->bitArray_[bit_index] & bit_mask[bit]) != bit_mask[bit]) {
			return false;
		}
	}
	return true;
}

std::size_t FSBloomFilter::containsAll(const unsigned char *keys,
		const std::size_t count) const {
	uint64_t hashes[count * this->functionCount_];
	for (std::size_t i = 0; i < count; i++) {
		for (std::size_t j = 0; j < this->functionCount_; j++) {
			hashes[i * this->functionCount_ + j]
					= this->hashFunction_->operator ()(
							keys
									+ (this->cryptoHashFunction_.getHashSize()
											* i),
							this->cryptoHashFunction_.getHashSize(), j)
							% this->filterSize_;
		}
	}
	std::sort(hashes, hashes + (count * this->functionCount_));
	std::size_t bit_index = 0;
	std::size_t bit = 0;
	for (std::size_t i = 0; i < count * this->functionCount_; i++) {
		compute_indices(hashes[i], bit_index, bit);
		if ((this->bitArray_[bit_index] & bit_mask[bit]) != bit_mask[bit]) {
			return i == 0 ? 1 : i;
		}
	}
	return true;
}

void FSBloomFilter::compute_indices(const uint64_t hash,
		std::size_t& bit_index, std::size_t& bit) const {
	bit_index = (hash % this->filterSize_) / BYTESIZE;
	bit = hash % BYTESIZE;
}

AbstractBloomFilter& FSBloomFilter::operator &=(
		const AbstractBloomFilter& filter) {
	/* intersection */
	const FSBloomFilter& filter_ = dynamic_cast<const FSBloomFilter&> (filter);
	if (this->filterSize_ == filter_.filterSize_) {
		for (std::size_t i = 0; i < this->mmapLength_; ++i) {
			this->bitArray_[i] &= filter_.bitArray_[i];
		}
	}
	return *this;
}

AbstractBloomFilter& FSBloomFilter::operator |=(
		const AbstractBloomFilter& filter) {
	/* union */
	const FSBloomFilter& filter_ = dynamic_cast<const FSBloomFilter&> (filter);
	if (this->filterSize_ == filter_.filterSize_) {
		for (std::size_t i = 0; i < this->mmapLength_; ++i) {
			this->bitArray_[i] |= filter_.bitArray_[i];
		}
	}
	return *this;
}

AbstractBloomFilter& FSBloomFilter::operator ^=(
		const AbstractBloomFilter& filter) {
	/* difference */
	const FSBloomFilter& filter_ = dynamic_cast<const FSBloomFilter&> (filter);
	if (this->filterSize_ == filter_.filterSize_) {
		for (std::size_t i = 0; i < this->mmapLength_; ++i) {
			this->bitArray_[i] ^= filter_.bitArray_[i];
		}
	}
	return *this;
}

bool FSBloomFilter::operator ==(const AbstractBloomFilter& filter) const {
	/* equals */
	try {
		const FSBloomFilter& filter_ =
				dynamic_cast<const FSBloomFilter&> (filter);
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
		c1 = c1 >> (BYTESIZE - remainder);
		c2 = c2 >> (BYTESIZE - remainder);
		return c1 == c2;
	} catch (const std::bad_cast& e) {
		return false;
	}
}

bool FSBloomFilter::operator !=(const AbstractBloomFilter& filter) const {
	/* diffs */
	try {
		const FSBloomFilter& filter_ =
				dynamic_cast<const FSBloomFilter&> (filter);

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
		c1 = c1 >> (BYTESIZE - remainder);
		c2 = c2 >> (BYTESIZE - remainder);
		return c1 != c2;
	} catch (const std::bad_cast& e) {
		return false;
	}
}

std::string FSBloomFilter::toString() {
	return utils::OutputFunctions::ArrayToBitString(this->bitArray_,
			this->filterSize_);
}
}

}
