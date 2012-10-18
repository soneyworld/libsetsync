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
					hashsize), db_(db) {
	/*
	 * Loading all set bloom filter bits from db
	 */
	// Cursor to read sequentially the db
	Dbc *cursorp;
	this->db_->cursor(NULL, &cursorp, 0);
	// Loaded position in the bloom filter
	uint64_t pos;
	// Buffer
	unsigned char hash[this->hashsize_];
	Dbt key, data;
	key.set_data(&pos);
	key.set_size(sizeof(uint64_t));
	data.set_data(hash);
	data.set_ulen(this->hashsize_);
	data.set_flags(DB_DBT_USERMEM);
	int ret;
	// Iterate over the database, retrieving each key in turn.;
	while ((ret = cursorp->get(&key, &data, DB_NEXT_NODUP)) == 0) {
		// Taking the returned key
		pos = *((uint64_t *) key.get_data());
		std::size_t bit_index = 0;
		std::size_t bit = 0;
		compute_indices(pos, bit_index, bit);
		// Sets the found bit to the filter
		this->bitArray_[bit_index] |= bit_mask[bit];
	}
	if (ret != DB_NOTFOUND) {
		// ret should be DB_NOTFOUND upon exiting the loop.
		// Dbc::get() will by default throw an exception if any
		// significant errors occur, so by default this if block
		// can never be reached.
	}
	// closing the loading cursor
	cursorp->close();
}

void DBBloomFilter::add(const unsigned char * key) {
	// take the given key as value for new db entries
	Dbt value(const_cast<unsigned char*> (key), this->hashsize_);
	// Insert the given key once per hash function
	for (int i = 0; i < this->functionCount_; i++) {
		// calculate the db key
		uint64_t pos = this->hashFunction_->hash(key, this->hashsize_, i);
		pos = pos % this->filterSize_;
		Dbt db_key(&pos, sizeof(uint64_t));
		// putting <pos/key> as key/value pair to berkeley db
		int ret = this->db_->put(NULL, &db_key, &value, DB_OVERWRITE_DUP);
		if (ret == DB_KEYEXIST) {
			// Abort: given key already exists in the db
			return;
		}
	}
	// finally adding the key also to the bloom filter
	FSBloomFilter::add(key);
}

void DBBloomFilter::addAll(const unsigned char* keys, const std::size_t count) {
	for(std::size_t i = 0; i < count; i++){
		add(keys+this->hashsize_*i);
	}
}

bool DBBloomFilter::remove(const unsigned char * key) {
	// Check, if the key has been added to the filter
	if (!contains(key)) {
		// Abort: key doesn't exists in bloom filter
		return false;
	}
	// success status of the remove process
	bool result = false;
	int ret;
	// cursor to iterate over multiple saved crypto keys
	Dbc *cursorp;
	// value buffer for found crypto keys
	unsigned char hash[this->hashsize_];
	// position in the bloom filter
	uint64_t pos;
	uint64_t temphash;
	Dbt db_key, db_data;
	db_data.set_data(hash);
	db_data.set_ulen(this->hashsize_);
	db_data.set_flags(DB_DBT_USERMEM);
	// Getting crypto key entries for all hash functions
	for (int func = 0; func < functionCount_; func++) {
		// The searched crypto hash has been found
		bool hash_found = false;
		// Another crypto hash has been found
		bool other_hash_found = false;
		// create new cursor
		this->db_->cursor(NULL, &cursorp, 0);
		temphash = this->hashFunction_->hash(key, this->hashsize_, func);
		pos = temphash % this->filterSize_;
		db_key.set_data(&pos);
		db_key.set_size(sizeof(uint64_t));
		// db request for the set of crypto keys saved for pos
		ret = cursorp->get(&db_key, &db_data, DB_SET);
		if (ret == 0) {
			// Prove, if the returned crypto key is equal to the given key
			memcpy(hash, db_data.get_data(), this->hashsize_);
			if (memcmp(key, hash, this->hashsize_) == 0) {
				hash_found = true;
				// Delete this entry from db
				cursorp->del(0);
			} else {
				// There are possibly multiple entries for pos
				other_hash_found = true;
			}
			// Size of the set of saved crypto key for pos
			db_recno_t counter;
			int r = cursorp->count(&counter, 0);
			if (counter > 1 && !hash_found) {
				// Iterating over the set to find the correct entry
				while ((ret = cursorp->get(&db_key, &db_data, DB_NEXT_DUP))
						== 0) {
					memcpy(hash, db_data.get_data(), this->hashsize_);
					if (memcmp(key, hash, this->hashsize_) == 0) {
						// correct crypto key found
						hash_found = true;
						cursorp->del(0);
					} else {
						// There are definitely multiple crypto keys for pos
						other_hash_found = true;
					}
				}
			} else if (counter > 1 && hash_found) {
				// There are definitely multiple crypto keys for pos
				other_hash_found = true;
			}
		}
		if (ret != DB_NOTFOUND) {
			// ret should be DB_NOTFOUND upon exiting the loop.
			// Dbc::get() will by default throw an exception if any
			// significant errors occur, so by default this if block
			// can never be reached.
		}
		// close the db cursor
		cursorp->close();
		// delete the bloom filter bit only if the found set of entries is now 0
		if (hash_found && !other_hash_found) {
			std::size_t bit_index = 0;
			std::size_t bit = 0;
			compute_indices(temphash, bit_index, bit);
			// sets bloom filter bit to 0
			this->bitArray_[bit_index] ^= bit_mask[bit];
		}
		if (hash_found) {
			// Operation has been successful
			result = true;
		}
	}
	return result;
}

void DBBloomFilter::clear() {
	u_int32_t counter;
	int ret = this->db_->truncate(NULL, &counter, 0);
	FSBloomFilter::clear();
}

DBBloomFilter::~DBBloomFilter() {
}

}
