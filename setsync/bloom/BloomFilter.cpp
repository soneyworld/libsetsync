/*
 * BloomFilter.cpp
 *
 *      Author: Till Lorentzen
 */

#include "BloomFilter.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define BYTESIZE 8

namespace bloom {

BloomFilter::BloomFilter(const BloomFilter& filter) {
	throw "MUST BE IMPLEMENTED";
}

BloomFilter::BloomFilter(const uint64_t numberOfElements,
		const bool hardMaximum, const float falsePositiveRate,
		const std::size_t hashsize) {
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
	memset(this->bitArray_, 0, (this->filterSize_ + (BYTESIZE - 1)) / BYTESIZE);
	this->itemCount_ = 0;
}

BloomFilter::~BloomFilter() {
	if (this->bitArray_ != NULL)
		free(this->bitArray_);
}

std::size_t BloomFilter::size() const {
	throw "MUST BE IMPLEMENTED";
}

BloomFilter& BloomFilter::operator &=(const BloomFilter& filter) {
	/* intersection */
	throw "MUST BE IMPLEMENTED";
}

BloomFilter& BloomFilter::operator |=(const BloomFilter& filter) {
	/* union */
	throw "MUST BE IMPLEMENTED";
}

BloomFilter& BloomFilter::operator ^=(const BloomFilter& filter) {
	/* difference */
	throw "MUST BE IMPLEMENTED";
}

bool BloomFilter::operator ==(const BloomFilter& filter) {
	/* equals */
	throw "MUST BE IMPLEMENTED";
}

bool BloomFilter::operator !=(const BloomFilter& filter) {
	/* diffs */
	throw "MUST BE IMPLEMENTED";
}

bool BloomFilter::operator!() const {
	throw "MUST BE IMPLEMENTED";
}

void BloomFilter::clear() {
	memset(this->bitArray_, 0, (this->filterSize_ + (BYTESIZE - 1)) / BYTESIZE);
	this->itemCount_ = 0;
}

void BloomFilter::add(const unsigned char *key) {
	if (this->hardMaximum_ && this->itemCount_ >= maxElements_)
		throw "Maximum of Elements reached, adding failed";
	throw "MUST BE IMPLEMENTED";
}

void BloomFilter::load(std::istream &in) {
	throw "MUST BE IMPLEMENTED";
}

bool BloomFilter::contains(const unsigned char *key) const {
	throw "MUST BE IMPLEMENTED";
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
}
