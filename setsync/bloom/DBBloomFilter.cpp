/*
 * DBBloomFilter.cpp
 *
 *      Author: Till Lorentzen
 */

#include "DBBloomFilter.h"

namespace bloom {

DBBloomFilter::DBBloomFilter(const uint64_t maxNumberOfElements,
		const bool hardMaximum, const float falsePositiveRate,
		const std::size_t hashsize) :
			FSBloomFilter(maxNumberOfElements, hardMaximum, falsePositiveRate,
					hashsize) {
	this->db_ = new Db(NULL, 0);
	u_int32_t oFlags = DB_CREATE; // Open flags;
	try {
		// Open the database
		this->db_->open(NULL, "bloom.db", NULL, DB_QUEUE, oFlags, 0);
	} catch (DbException &e) {
		this->db_ = NULL;
		// Error handling code goes here
	} catch (std::exception &e) {
		this->db_ = NULL;
		// Error handling code goes here
	}
}

void DBBloomFilter::add(const unsigned char * key){
	FSBloomFilter::add(key);
}

DBBloomFilter::~DBBloomFilter() {
	try {
		if (this->db_ != NULL) {
			this->db_->remove("bloom.db", NULL, 0);
			this->db_->close(0);
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
