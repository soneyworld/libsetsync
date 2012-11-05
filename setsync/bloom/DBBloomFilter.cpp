/*
 * DBBloomFilter.cpp
 *
 *      Author: Till Lorentzen
 */

#include "DBBloomFilter.h"
#include <db_cxx.h>
#include <string.h>
#include <setsync/utils/OutputFunctions.h>
#include <setsync/utils/bitset.h>

namespace bloom {

const char DBBloomFilter::setting_name[] = "bloom_setting";

const std::size_t DbBloomFilterSetting::getBufferSize() {
	return sizeof(uint64_t) + sizeof(float) + sizeof(std::size_t)
			+ sizeof(bool);
}

void DbBloomFilterSetting::marshall(void * target,
		const DbBloomFilterSetting& toBeMarshalled) {
	unsigned char * t = (unsigned char*) target;
	unsigned int pos = 0;
	memcpy(t + pos, &(toBeMarshalled.maxNumberOfElements), sizeof(uint64_t));
	pos += sizeof(uint64_t);
	memcpy(t + pos, &(toBeMarshalled.hashSize), sizeof(std::size_t));
	pos += sizeof(std::size_t);
	memcpy(t + pos, &(toBeMarshalled.falsePositiveRate), sizeof(float));
	pos += sizeof(float);
	memcpy(t + pos, &(toBeMarshalled.hardMaximum), sizeof(bool));
}

DbBloomFilterSetting::DbBloomFilterSetting(const uint64_t maxNumberOfElements,
		const bool hardMaximum, const float falsePositiveRate,
		const std::size_t hashsize) :
	maxNumberOfElements(maxNumberOfElements), hardMaximum(hardMaximum),
			falsePositiveRate(falsePositiveRate), hashSize(hashsize) {
}

DbBloomFilterSetting::DbBloomFilterSetting(void * toLoad) {
	unsigned char * load = (unsigned char*) toLoad;
	unsigned int pos = 0;
	memcpy(&(this->maxNumberOfElements), load + pos, sizeof(uint64_t));
	pos += sizeof(uint64_t);
	memcpy(&(this->hashSize), load + pos, sizeof(std::size_t));
	pos += sizeof(std::size_t);
	memcpy(&(this->falsePositiveRate), load + pos, sizeof(float));
	pos += sizeof(float);
	memcpy(&(this->hardMaximum), load + pos, sizeof(bool));
}

DBBloomFilter::DBBloomFilter(const utils::CryptoHash& hash, Db * db,
		const uint64_t maxNumberOfElements, const bool hardMaximum,
		const float falsePositiveRate) :
			AbstractBloomFilter(hash),
			FSBloomFilter(hash, maxNumberOfElements, hardMaximum,
					falsePositiveRate), CountingBloomFilter(hash), db_(db) {
	/*
	 * Loading all set bloom filter bits from db
	 */
	// Cursor to read sequentially the db
	Dbc *cursorp;
	this->db_->cursor(NULL, &cursorp, 0);
	// Loaded position in the bloom filter
	uint64_t pos;
	// Buffer
	unsigned char _hash[this->cryptoHashFunction_.getHashSize()];
	Dbt key, data;
	key.set_data(&pos);
	key.set_size(sizeof(uint64_t));
	data.set_data(_hash);
	data.set_ulen(this->cryptoHashFunction_.getHashSize());
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
	Dbt value(const_cast<unsigned char*> (key),
			this->cryptoHashFunction_.getHashSize());
	// Insert the given key once per hash function
	for (int i = 0; i < this->functionCount_; i++) {
		// calculate the db key
		uint64_t pos = this->hashFunction_->operator ()(key,
				this->cryptoHashFunction_.getHashSize(), i);
		pos = pos % this->filterSize_;
		Dbt db_key(&pos, sizeof(uint64_t));
		// putting <pos/key> as key/value pair to berkeley db
		int ret = this->db_->put(NULL, &db_key, &value, DB_NODUPDATA);
		if (ret == DB_KEYEXIST) {
			// Abort: given key already exists in the db
			return;
		}
	}
	// finally adding the key also to the bloom filter
	FSBloomFilter::add(key);
}

void DBBloomFilter::addAll(const unsigned char* keys, const std::size_t count) {
	for (std::size_t i = 0; i < count; i++) {
		add(keys + this->cryptoHashFunction_.getHashSize() * i);
	}
}

bool DBBloomFilter::remove(const unsigned char * key) {
	DbEnv * env = this->db_->get_env();
	DbTxn * tid = NULL;
	u_int32_t env_flags;
	env->get_open_flags(&env_flags);
	if ((env_flags & DB_INIT_TXN) == DB_INIT_TXN) {
		env->txn_begin(NULL, &tid, 0);
	}
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
	unsigned char hash[this->cryptoHashFunction_.getHashSize()];
	// position in the bloom filter
	uint64_t pos;
	Dbt db_key, db_data;
	db_data.set_data(hash);
	db_data.set_ulen(this->cryptoHashFunction_.getHashSize());
	db_data.set_flags(DB_DBT_USERMEM);
	// create new cursor
	this->db_->cursor(tid, &cursorp, 0);
	// Getting crypto key entries for all hash functions
	for (int func = 0; func < functionCount_; func++) {
		// The searched crypto hash has been found
		bool hash_found = false;
		// Another crypto hash has been found
		bool other_hash_found = false;
		pos = this->hashFunction_->operator ()(key,
				this->cryptoHashFunction_.getHashSize(), func);
		pos = pos % this->filterSize_;
		db_key.set_data(&pos);
		db_key.set_size(sizeof(uint64_t));
		// db request for the set of crypto keys saved for pos
		ret = cursorp->get(&db_key, &db_data, DB_SET);
		if (ret == 0) {
			// Size of the set of saved crypto key for pos
			db_recno_t counter;
			int r = cursorp->count(&counter, 0);
			// Prove, if the returned crypto key is equal to the given key
			memcpy(hash, db_data.get_data(),
					this->cryptoHashFunction_.getHashSize());
			if (memcmp(key, hash, this->cryptoHashFunction_.getHashSize()) == 0) {
				hash_found = true;
				// Delete this entry from db
				cursorp->del(0);
			} else {
				// There are possibly multiple entries for pos
				other_hash_found = true;
			}

			if (counter > 1 && !hash_found) {
				// Iterating over the set to find the correct entry
				while ((ret = cursorp->get(&db_key, &db_data, DB_NEXT_DUP))
						== 0) {
					memcpy(hash, db_data.get_data(),
							this->cryptoHashFunction_.getHashSize());
					if (memcmp(key, hash,
							this->cryptoHashFunction_.getHashSize()) == 0) {
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
	// close the db cursor
	cursorp->close();
	if (tid != NULL) {
		if (result)
			tid->commit(0);
		else
			tid->abort();
	}
	return result;
}

void DBBloomFilter::clear() {
	u_int32_t counter;
	int ret = this->db_->truncate(NULL, &counter, 0);
	FSBloomFilter::clear();
}

void DBBloomFilter::diff(const unsigned char * externalBF,
		const std::size_t length, const std::size_t offset,
		setsync::AbstractDiffHandler& handler) const {
	if (length + offset > this->mmapLength_)
		throw "";
	unsigned char c;
	// Cursor to read sequentially the db
	Dbc *cursorp;
	this->db_->cursor(NULL, &cursorp, 0);
	for (std::size_t i = 0; i < length; ++i) {
		for (unsigned short j = 0; j < 8; j++) {
			if (!BITTEST(externalBF+i,j) && BITTEST(this->bitArray_+i+offset,j)) {
				// Loaded position in the bloom filter
				uint64_t pos = (offset + i) * 8 + j;
				if (pos > this->filterSize_)
					continue;
				// Buffer
				unsigned char hash[this->cryptoHashFunction_.getHashSize()];
				Dbt key, data;
				key.set_data(&pos);
				key.set_size(sizeof(uint64_t));
				data.set_data(hash);
				data.set_ulen(this->cryptoHashFunction_.getHashSize());
				data.set_flags(DB_DBT_USERMEM);
				// Request first entry
				int ret = cursorp->get(&key, &data, DB_SET);
				if (ret == DB_NOTFOUND) {
					cursorp->close();
					throw DbException(ret);
				} else {
					handler((unsigned char*) data.get_data(),
							this->cryptoHashFunction_.getHashSize());
				}
				// Iterate over duplicated entries
				while ((ret = cursorp->get(&key, &data, DB_NEXT_DUP)) == 0) {
					// Call handler for the found hash
					handler((unsigned char*) data.get_data(),
							this->cryptoHashFunction_.getHashSize());
				}
				if (ret != DB_NOTFOUND) {
					// ret should be DB_NOTFOUND upon exiting the loop.
					// Dbc::get() will by default throw an exception if any
					// significant errors occur, so by default this block
					// can never be reached.
				}
			}
		}
	}
	// closing the loading cursor
	cursorp->close();
}

DBBloomFilter::~DBBloomFilter() {
}

const DbBloomFilterSetting DBBloomFilter::loadSettings(Db * db) {
	Dbt key(const_cast<char *> (setting_name), strlen(setting_name));
	unsigned char result[DbBloomFilterSetting::getBufferSize()];
	Dbt data;
	data.set_data(&result);
	data.set_flags(DB_DBT_USERMEM);
	data.set_ulen(DbBloomFilterSetting::getBufferSize());
	int ret = db->get(NULL, &key, &data, 0);
	if (ret == 0) {
		DbBloomFilterSetting settings(data.get_data());
		return settings;
	} else {
		throw DbException(ret);
	}
}

void DBBloomFilter::saveSettings(Db * db, const uint64_t maxNumberOfElements,
		const bool hardMaximum, const float falsePositiveRate,
		const std::size_t hashsize) {
	unsigned char buffer[DbBloomFilterSetting::getBufferSize()];
	Dbt key(const_cast<char *> (setting_name), strlen(setting_name));
	Dbt data(buffer, DbBloomFilterSetting::getBufferSize());
	DbBloomFilterSetting toSave(maxNumberOfElements, hardMaximum,
			falsePositiveRate, hashsize);
	DbBloomFilterSetting::marshall(buffer, toSave);
	db->del(NULL, &key, 0);
	db->put(NULL, &key, &data, DB_OVERWRITE_DUP);
}

}
