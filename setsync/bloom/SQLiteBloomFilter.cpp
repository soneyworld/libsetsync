/*
 * SQLiteBloomFilter.cpp
 *
 *      Author: Till Lorentzen
 */

#include "SQLiteBloomFilter.h"
#include <limits>

namespace bloom {

SQLiteBloomFilter::SQLiteBloomFilter(sqlite::SQLiteDatabase * db,
		const std::string hashFunction, const uint64_t maxNumberOfElements,
		const bool hardMaximum, const float falsePositiveRate,
		const std::size_t hashsize) :
	BloomFilter(hashFunction, maxNumberOfElements, hardMaximum, falsePositiveRate, hashsize) {
	this->tempDatabase = false;
	this->db = db;
	std::string tablename = "revindex";
	this->index = new sqlite::SQLiteIndex(this->db, tablename);
}
SQLiteBloomFilter::SQLiteBloomFilter(const std::string hashFunction,
		const uint64_t maxNumberOfElements, const bool hardMaximum,
		const float falsePositiveRate, const std::size_t hashsize) :
	BloomFilter(hashFunction, maxNumberOfElements, hardMaximum, falsePositiveRate, hashsize) {
	this->tempDatabase = true;
	this->db = new sqlite::SQLiteDatabase();
	std::string tablename = "revindex";
	this->index = new sqlite::SQLiteIndex(this->db, tablename);
}

SQLiteBloomFilter::SQLiteBloomFilter(const uint64_t maxNumberOfElements, const bool hardMaximum,
		const float falsePositiveRate, const std::size_t hashsize) :
	BloomFilter(maxNumberOfElements, hardMaximum, falsePositiveRate, hashsize) {
	this->tempDatabase = true;
	this->db = new sqlite::SQLiteDatabase();
	std::string tablename = "revindex";
	this->index = new sqlite::SQLiteIndex(this->db, tablename);
}

void SQLiteBloomFilter::add(const unsigned char *key) {
	if (this->hardMaximum_ && this->itemCount_ >= maxElements_)
		throw "Maximum of Elements reached, adding failed";
	std::size_t bit_index = 0;
	std::size_t bit = 0;
	for (int i = 0; i < this->functionCount_; i++) {
		uint64_t pos = this->hashFunction_->hash(key, SHA_DIGEST_LENGTH, i);
		compute_indices(pos, bit_index, bit);
		this->bitArray_[bit_index / BYTESIZE] |= bit_mask[bit];
		pos = bit_index << 8 + bit;
		this->index->insert(pos, key, this->hashsize_);
	}
	if (this->itemCount_ < std::numeric_limits<uint64_t>::max())
		this->itemCount_++;
}

SQLiteBloomFilter::~SQLiteBloomFilter() {
	delete this->index;
	if (this->tempDatabase)
		delete this->db;
}

}
