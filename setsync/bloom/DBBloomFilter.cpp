/*
 * DBBloomFilter.cpp
 *
 *      Author: Till Lorentzen
 */

#include "DBBloomFilter.h"
#include <db_cxx.h>
#include <string.h>
#include <setsync/utils/OutputFunctions.h>

namespace bloom {

DBBloomFilter::DBBloomFilter(Db * db, const uint64_t maxNumberOfElements,
		const bool hardMaximum, const float falsePositiveRate,
		const std::size_t hashsize) :
			FSBloomFilter(maxNumberOfElements, hardMaximum, falsePositiveRate,
					hashsize) {
	this->db_ = db;
	Dbc *cursorp;
	this->db_->cursor(NULL, &cursorp, 0);
	uint64_t pos;
	unsigned char hash[this->hashsize_];
	Dbt key, data;
	key.set_data(&pos);
	key.set_size(sizeof(uint64_t));
	data.set_data(hash);
	data.set_ulen(this->hashsize_);
	data.set_flags(DB_DBT_USERMEM);
	int ret;
	// Iterate over the database, retrieving each record in turn.
	key.set_size(sizeof(uint64_t));
	while ((ret = cursorp->get(&key, &data, DB_NEXT_NODUP)) == 0) {
		// Do interesting things with the Dbts here.
		std::size_t bit_index = 0;
		std::size_t bit = 0;
		compute_indices(pos, bit_index, bit);
		this->bitArray_[bit_index] |= bit_mask[bit];
	}
	if (ret != DB_NOTFOUND) {
		// ret should be DB_NOTFOUND upon exiting the loop.
		// Dbc::get() will by default throw an exception if any
		// significant errors occur, so by default this if block
		// can never be reached.
	}

	cursorp->close();
}

void DBBloomFilter::add(const unsigned char * key) {
	FSBloomFilter::add(key);
	Dbt value(const_cast<unsigned char*>(key), this->hashsize_);
	for (int i = 0; i < this->functionCount_; i++) {
		uint64_t pos = this->hashFunction_->hash(key, this->hashsize_, i);
		pos = pos % this->filterSize_;
		Dbt db_key(&pos, sizeof(uint64_t));
		int ret = this->db_->put(NULL, &db_key, &value, DB_OVERWRITE_DUP);
		if (ret == DB_KEYEXIST) {
			return;
		}
	}
}

bool DBBloomFilter::remove(const unsigned char * key) {
	if (!contains(key)) {
		return false;
	}
	bool result = false;
	int ret;
	Dbc *cursorp;
	unsigned char hash[this->hashsize_];
	uint64_t pos;
	uint64_t temphash;
	Dbt db_key, db_data;
	db_data.set_data(hash);
	db_data.set_ulen(this->hashsize_);
	db_data.set_flags(DB_DBT_USERMEM);
	std::cout << "remove " << utils::OutputFunctions::CryptoHashtoString(key)
			<< std::endl;
	for (int func = 0; func < functionCount_; func++) {
		bool hash_found = false;
		bool other_hash_found = false;
		this->db_->cursor(NULL, &cursorp, 0);
		temphash = this->hashFunction_->hash(key, this->hashsize_, func);
		pos = temphash % this->filterSize_;
		db_key.set_data(&pos);
		db_key.set_size(sizeof(uint64_t));
		ret = cursorp->get(&db_key, &db_data, DB_SET);
		std::cout << "function " << func << ": " << pos;
		if (ret == 0) {
			std::cout << " " << utils::OutputFunctions::CryptoHashtoString(
					(const unsigned char*) db_data.get_data());
			memcpy(hash, db_data.get_data(), this->hashsize_);
			if (memcmp(key, hash, this->hashsize_) == 0) {
				hash_found = true;
			} else {
				other_hash_found = true;
			}
			db_recno_t counter;
			int r = cursorp->count(&counter, 0);
			if (counter > 1 && !hash_found) {
				while ((ret = cursorp->get(&db_key, &db_data, DB_NEXT_DUP))
						== 0) {
					// Do interesting things with the Dbts here.
					memcpy(hash, db_data.get_data(), this->hashsize_);
					if (memcmp(key, hash, this->hashsize_) == 0) {
						hash_found = true;
					} else {
						other_hash_found = true;
					}
				}
			} else if (counter > 1 && hash_found) {
				other_hash_found = true;
			}
		}
		if (ret != DB_NOTFOUND) {
			// ret should be DB_NOTFOUND upon exiting the loop.
			// Dbc::get() will by default throw an exception if any
			// significant errors occur, so by default this if block
			// can never be reached.
		}
		cursorp->close();
		if (hash_found && !other_hash_found) {
			std::cout << " bit reset";
			std::size_t bit_index = 0;
			std::size_t bit = 0;
			compute_indices(temphash, bit_index, bit);
			this->bitArray_[bit_index] ^= bit_mask[bit];
		}
		if (hash_found) {
			std::cout << " hash found";
			result = true;
		}
		std::cout << std::endl;
	}
	return result;
}

DBBloomFilter::~DBBloomFilter() {
}

}
