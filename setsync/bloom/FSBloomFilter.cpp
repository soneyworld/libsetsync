/*
 * FSBloomFilter.cpp
 *
 *      Author: Till Lorentzen
 */

#include "FSBloomFilter.h"
#include <limits>

namespace bloom {

FSBloomFilter::FSBloomFilter(const uint64_t maxNumberOfElements,
		const bool hardMaximum, const float falsePositiveRate,
		const std::size_t hashsize) {

}

FSBloomFilter::~FSBloomFilter() {
}

void FSBloomFilter::load(std::istream &in, const uint64_t numberOfElements) {

}
uint64_t FSBloomFilter::save(std::ostream &out) {

}
void FSBloomFilter::clear() {

}

void FSBloomFilter::add(const unsigned char *key) {
	if (this->hardMaximum_ && this->itemCount_ >= maxElements_)
		throw "Maximum of Elements reached, adding failed";
	std::size_t bit_index = 0;
	std::size_t bit = 0;
	for (int i = 0; i < this->functionCount_; i++) {
		uint64_t pos = this->hashFunction_->hash(key, SHA_DIGEST_LENGTH, i);
		compute_indices(pos, bit_index, bit);
		this->bitArray_[bit_index / BYTESIZE] |= bit_mask[bit];
	}
	if (this->itemCount_ < std::numeric_limits<uint64_t>::max())
		this->itemCount_++;
}

bool FSBloomFilter::contains(const unsigned char *key) const {
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

void FSBloomFilter::compute_indices(const uint64_t hash,
		std::size_t& bit_index, std::size_t& bit) const {
	bit_index = hash % this->filterSize_;
	bit = bit_index % BYTESIZE;
}
}
