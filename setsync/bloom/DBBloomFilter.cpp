/*
 * DBBloomFilter.cpp
 *
 *      Author: Till Lorentzen
 */

#include "DBBloomFilter.h"
#include <db_cxx.h>
#include <string.h>

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
	unsigned char v[this->hashsize_];
	memcpy(v, key, this->hashsize_);
	Dbt value(v, this->hashsize_);
	for (int i = 0; i < this->functionCount_; i++) {
		uint64_t pos = this->hashFunction_->hash(key, this->hashsize_, i);
		Dbt db_key;
		db_key.set_data(&pos);
		db_key.set_ulen(sizeof(uint64_t));
		db_key.set_flags(DB_DBT_USERMEM);
		int ret = this->db_->put(NULL, &db_key, &value, DB_NODUPDATA);
		if (ret == DB_KEYEXIST) {
			return;
		}
	}
}

bool DBBloomFilter::remove(const unsigned char * key) {
	bool result = false;
	int ret;
	Dbc *cursorp;
	unsigned char hash[this->hashsize_];
	uint64_t pos;
	Dbt db_key, db_data;
	db_data.set_data(hash);
	db_data.set_ulen(this->hashsize_);
	db_data.set_flags(DB_DBT_USERMEM);
	for (int func = 0; func < functionCount_; func++) {
		bool hash_found = false;
		bool other_hash_found = false;
		this->db_->cursor(NULL, &cursorp, 0);
		pos = this->hashFunction_->hash(key, this->hashsize_, func);
		db_key.set_data(&pos);
		db_key.set_size(sizeof(uint64_t));
		ret = cursorp->get(&db_key, &db_data, DB_SET);
		if (ret == 0) {
			if (memcmp(key, db_data.get_data(), this->hashsize_) == 0) {
				hash_found = true;
			} else {
				other_hash_found = true;
			}
			if (cursorp->count(NULL, 0) > 1 && !hash_found) {
				while ((ret = cursorp->get(&db_key, &db_data, DB_NEXT_DUP))
						== 0) {
					// Do interesting things with the Dbts here.
					if (memcmp(key, db_data.get_data(), this->hashsize_) == 0) {
						hash_found = true;
					} else {
						other_hash_found = true;
					}
				}
			} else if(cursorp->count(NULL, 0) > 1 && hash_found) {
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
			std::size_t bit_index = 0;
			std::size_t bit = 0;
			compute_indices(pos, bit_index, bit);
			this->bitArray_[bit_index] |= bit_mask[bit];
		}
		if (hash_found) {
			result = true;
		}
	}
	if (result) {

	}
	return result;
}

DBBloomFilter::~DBBloomFilter() {
}

}
