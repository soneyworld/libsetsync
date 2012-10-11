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
		int ret = this->db_->put(NULL, &db_key, &value, 0);
		if (ret == DB_KEYEXIST) {
			return;
		}
	}
}

bool DBBloomFilter::remove(const unsigned char * key) {
	Dbc *cursorp;
	this->db_->cursor(NULL, &cursorp, 0);
	unsigned char hash[this->hashsize_];
	uint64_t pos;
	Dbt db_key, data;
	db_key.set_data(&pos);
	db_key.set_size(sizeof(uint64_t));
	data.set_data(hash);
	data.set_ulen(this->hashsize_);
	data.set_flags(DB_DBT_USERMEM);
	int ret;
	// Iterate over the database, retrieving each record in turn.
	db_key.set_size(sizeof(uint64_t));
	while ((ret = cursorp->get(&db_key, &data, DB_NEXT_NODUP)) == 0) {
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
	//TODO
	return false;
}

DBBloomFilter::~DBBloomFilter() {}

}
