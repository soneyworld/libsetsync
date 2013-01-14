/*
 * KeyValueCountingBloomFilter.cpp
 *
 *      Author: Till Lorentzen
 */

#include "KeyValueCountingBloomFilter.h"
#include <setsync/utils/bitset.h>
#include <stdlib.h>
namespace setsync {
namespace bloom {

const char KeyValueCountingBloomFilter::sizeKey[] = "bfsize";

KeyValueCountingBloomFilter::KeyValueCountingBloomFilter(
		const crypto::CryptoHash& hash,
		setsync::storage::AbstractKeyValueStorage& storage,
		const std::string& file, const uint64_t maxNumberOfElements,
		const bool hardMaximum, const float falsePositiveRate) :
			AbstractBloomFilter(hash),
			CountingBloomFilter(hash),
			FSBloomFilter(hash, (file.size() == 0) ? NULL : file.c_str(),
					maxNumberOfElements, hardMaximum, falsePositiveRate),
			storage_(storage) {
	/*
	 * Loading all set bloom filter bits from db
	 */
	// Cursor to read sequentially the db
	setsync::storage::AbstractKeyValueIterator * iter =
			storage_.createIterator();
	iter->seekToFirst();
	uint64_t pos;
	while (iter->valid()) {
		if (iter->keySize() == sizeof(uint64_t)) {
			iter->key((unsigned char *) &pos);
			std::size_t bit_index = 0;
			std::size_t bit = 0;
			compute_indices(pos, bit_index, bit);
			// Sets the found bit to the filter
			this->bitArray_[bit_index] |= bit_mask[bit];
		} else if (iter->keySize() == strlen(sizeKey)) {
			uint64_t * size;
			size_t valuesize;
			if (this->storage_.get((unsigned char *) sizeKey, strlen(sizeKey),
					(unsigned char **) &size, &valuesize)) {
				if (valuesize == sizeof(uint64_t)) {
					this->itemCount_ = *size;
				}
				delete size;
			}
		}
		iter->next();
	}
	delete iter;
}

KeyValueCountingBloomFilter::~KeyValueCountingBloomFilter() {
	if (this->itemCount_ > 0) {
		this->storage_.put((unsigned char*) sizeKey, strlen(sizeKey),
				(unsigned char*) &itemCount_, sizeof(uint64_t));
	}
}
void KeyValueCountingBloomFilter::diff(const unsigned char * externalBF,
		const std::size_t length, const std::size_t offset,
		setsync::AbstractDiffHandler& handler) const {
	if (length + offset > this->mmapLength_)
		throw "";
	for (std::size_t i = 0; i < length; ++i) {
		for (unsigned short j = 0; j < 8; j++) {
			if (!BITTEST(externalBF+i,j) && BITTEST(this->bitArray_+i+offset,j)) {
				// Loaded position in the bloom filter
				uint64_t pos = (offset + i) * 8 + j;
				if (pos > this->filterSize_)
					continue;
				// Buffer
				unsigned char * resultbuffer;
				std::size_t resultSize;
				if (storage_.get((unsigned char*) &pos, sizeof(uint64_t),
						&resultbuffer, &resultSize)) {
					int numberOfResults = resultSize
							/ this->cryptoHashFunction_.getHashSize();
					unsigned char * pos = resultbuffer;
					for (int k = 0; k < numberOfResults; k++) {
						handler(pos, this->cryptoHashFunction_.getHashSize(),
								true);
						pos += this->cryptoHashFunction_.getHashSize();
					}
					free(resultbuffer);
				}
			}
		}
	}
}

std::size_t KeyValueCountingBloomFilter::getChunk(unsigned char * buffer,
		const std::size_t maxlength, std::size_t offset) {
	if (offset >= size()) {
		return 0;
	}
	std::size_t chunksize = std::min(maxlength, size() - offset);
	memcpy(buffer, this->bitArray_ + offset, chunksize);
	return chunksize;
}

void KeyValueCountingBloomFilter::add(const unsigned char * key) {
	unsigned char * resultbuffer;
	std::size_t resultSize;
	// Insert the given key once per hash function
	for (std::size_t i = 0; i < this->functionCount_; i++) {
		// calculate the db key
		uint64_t k = this->hashFunction_->operator ()(key,
				this->cryptoHashFunction_.getHashSize(), i);
		k = k % this->filterSize_;
		if (!this->storage_.get((unsigned char*) &k, sizeof(uint64_t),
				&resultbuffer, &resultSize)) {
			this->storage_.put((unsigned char*) &k, sizeof(uint64_t), key,
					this->cryptoHashFunction_.getHashSize());
			// putting <pos/key> as key/value pair to berkeley db
		} else {
			int numberOfResults = resultSize
					/ this->cryptoHashFunction_.getHashSize();
			for (int j = 0; j < numberOfResults; j++) {
				if (memcmp(
						resultbuffer + j
								* this->cryptoHashFunction_.getHashSize(), key,
						this->cryptoHashFunction_.getHashSize()) == 0) {
					// Key exists, so it can be skipped
					free(resultbuffer);
					return;
				}
			}
			unsigned char inputbuffer[resultSize
					+ this->cryptoHashFunction_.getHashSize()];
			memcpy(inputbuffer, resultbuffer, resultSize);
			memcpy(inputbuffer + resultSize, key,
					this->cryptoHashFunction_.getHashSize());
			this->storage_.put((unsigned char*) &k, sizeof(uint64_t),
					inputbuffer,
					resultSize + this->cryptoHashFunction_.getHashSize());
			free(resultbuffer);
		}
	}
	// finally adding the key also to the bloom filter
	FSBloomFilter::add(key);
}

void KeyValueCountingBloomFilter::clear() {
	this->storage_.clear();
	FSBloomFilter::clear();
}

void KeyValueCountingBloomFilter::addAll(const unsigned char* keys,
		const std::size_t count) {
	for (std::size_t i = 0; i < count; i++) {
		add(keys + this->cryptoHashFunction_.getHashSize() * i);
	}
}

bool KeyValueCountingBloomFilter::remove(const unsigned char * key) {
	// Check, if the key has been added to the filter
	if (!contains(key)) {
		// Abort: key doesn't exists in bloom filter
		return false;
	}
	unsigned char * resultbuffer;
	std::size_t resultSize;
	// success status of the remove process
	bool result = false;
	// position in the bloom filter
	uint64_t pos;
	// Getting crypto key entries for all hash functions
	for (std::size_t func = 0; func < functionCount_; func++) {
		// The searched crypto hash has been found
		bool hash_found = false;
		// Another crypto hash has been found
		bool other_hash_found = false;
		pos = this->hashFunction_->operator ()(key,
				this->cryptoHashFunction_.getHashSize(), func);
		pos = pos % this->filterSize_;
		if (this->storage_.get((unsigned char*) &pos, sizeof(uint64_t),
				&resultbuffer, &resultSize)) {
			int numberOfResults = resultSize
					/ this->cryptoHashFunction_.getHashSize();
			if (numberOfResults > 1) {
				other_hash_found = true;
				unsigned char * output_position = resultbuffer;
				for (int j = 0; j < numberOfResults; j++) {
					memcpy(
							output_position,
							resultbuffer + j
									* this->cryptoHashFunction_.getHashSize(),
							this->cryptoHashFunction_.getHashSize());
					if (memcmp(
							resultbuffer + j
									* this->cryptoHashFunction_.getHashSize(),
							key, this->cryptoHashFunction_.getHashSize()) != 0) {
						output_position
								+= this->cryptoHashFunction_.getHashSize();
					}
				}
				this->storage_.put((unsigned char *) &pos, sizeof(uint64_t),
						resultbuffer, resultSize - 1);
			} else {
				this->storage_.del((unsigned char *) &pos, sizeof(uint64_t));
				hash_found = true;
			}
			free(resultbuffer);
		}
		// delete the bloom filter bit only if the found set of entries is now 0
		if (hash_found && !other_hash_found) {
			std::size_t bit_index = 0;
			std::size_t bit = 0;
			compute_indices(pos, bit_index, bit);
			// sets bloom filter bit to 0
			BITCLEAR(this->bitArray_, pos);
		}
		if (hash_found) {
			// Operation has been successful
			result = true;
		}
	}
	return result;
}

}
}
