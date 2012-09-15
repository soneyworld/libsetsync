/*
 * BloomFilter.cpp
 *
 *      Author: Till Lorentzen
 */

#include "BloomFilter.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

namespace bloom {

BloomFilter::BloomFilter(const BloomFilter& filter) {
	throw "MUST BE IMPLEMENTED";
}

BloomFilter::BloomFilter(const uint64_t numberOfElements, const bool hardMaximum,
		const float falsePositiveRate) {
	this->bitArray_ = NULL;
	if (falsePositiveRate >= 1 || falsePositiveRate <= 0)
		throw "";
	this->hardMaximum_ = hardMaximum;
	this->filterSize_ = ceil(
			(numberOfElements * log(falsePositiveRate)) / log(
					1.0 / (pow(2.0, log(2.0)))));
	// Calculating the number of hash functions necessary to reach the given
	this->functionCount_ = round(
			log(2.0) * this->filterSize_ / numberOfElements);
	// Minimal count of hash functions has to be 1
	if(this->functionCount_ < 1)
		this->functionCount_ = 1;
	this->bitArray_ = (unsigned char *) malloc((this->filterSize_ + 7) / 8);
	memset(this->bitArray_, 0, (this->filterSize_ + 7) / 8);
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
	throw "MUST BE IMPLEMENTED";
}
}
