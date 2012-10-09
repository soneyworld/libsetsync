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
	u_int32_t oFlags = DB_CREATE; // Open flags;
	try {
		// Open the database
		this->db_->open(NULL, "bloom.db", "bloom", DB_QUEUE, oFlags, 0);
	} catch (DbException &e) {
		this->db_ = NULL;
		// Error handling code goes here
	} catch (std::exception &e) {
		this->db_ = NULL;
		// Error handling code goes here
	}
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
		int ret = this->db_->put(NULL, &db_key, &value, DB_NODUPDATA|DB_MULTIPLE);
		if (ret == DB_KEYEXIST) {
			return;
		}
	}
}

DBBloomFilter::~DBBloomFilter() {
	try {
		if (this->db_ != NULL) {
			this->db_->close(0);
			this->db_->remove("bloom.db", "bloom", 0);
		}
	} catch (DbException &e) {
		// Error handling code goes here
	} catch (std::exception &e) {
		// Error handling code goes here
	}
	if (this->db_ != NULL) {
		delete this->db_;
	}
}

}
