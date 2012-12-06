/*
 * BdbStorage.cpp
 *
 *      Author: Till Lorentzen
 */

#include "BdbStorage.h"
#include <setsync/utils/BerkeleyDB.h>
#include <stdlib.h>
#include <string.h>

namespace setsync {

namespace storage {

BdbIterator::BdbIterator(Db * db, DbTxn * parent) :
	valid_(true) {
	txn = NULL;
	if (utils::BerkeleyDB::isTransactionEnabled(db)) {
		db->get_env()->txn_begin(parent, &txn, 0);
	}
	db->cursor(txn, &cursorp, 0);
	seekToFirst();
}

BdbIterator::~BdbIterator() {
	cursorp->close();
	if (txn != NULL) {
		txn->commit(0);
	}
}

bool BdbIterator::valid() const {
	return valid_;
}

void BdbIterator::seekToFirst() {
	int ret = this->cursorp->get(&key_, &data_, DB_FIRST);
	if (ret == DB_NOTFOUND) {
		this->valid_ = false;
	} else if (ret == 0) {
		this->valid_ = true;
	}
}

void BdbIterator::next() {
	int ret = this->cursorp->get(&key_, &data_, DB_NEXT);
	if (ret == DB_NOTFOUND) {
		this->valid_ = false;
	} else if (ret == 0) {
		this->valid_ = true;
	}
}

size_t BdbIterator::keySize() const {
	return this->key_.get_size();
}

void BdbIterator::key(unsigned char * buffer) const {
	memcpy(buffer, this->key_.get_data(), this->key_.get_size());
}

BdbStorage::BdbStorage(Db * db) :
	db_(db), berkeley::AbstractBdbTableUser(db) {

}

BdbStorage::~BdbStorage() {
}

bool BdbStorage::get(const unsigned char * key, const std::size_t length,
		unsigned char ** value, std::size_t * valueSize) const {
	DbTxn * txn = NULL;
	int ret;
	Dbt k((char*) key, length);
	Dbt data;
	if (this->isTransactionEnabled()) {
		this->db_->get_env()->txn_begin(this->getParentTransaction(), &txn, 0);
	}
	ret = this->db_->get(txn, &k, &data, 0);
	if (this->isTransactionEnabled()) {
		if (ret != 0) {
			txn->abort();
		} else {
			txn->commit(0);
		}
	}
	if (ret == DB_NOTFOUND) {
		*value = NULL;
		*valueSize = 0;
		return false;
	} else if (ret != 0) {
		throw DbException(0);
	}
	*value = (unsigned char*) malloc(data.get_size());
	memcpy(*value, data.get_data(), data.get_size());
	*valueSize = data.get_size();
	return true;
}
void BdbStorage::put(const unsigned char * key, const std::size_t keySize,
		const unsigned char * value, const std::size_t valueSize) {
	int ret;
	DbTxn * txn = NULL;
	Dbt k((char*) key, keySize);
	Dbt data((char*)value,valueSize);
	data.set_flags(DB_DBT_USERMEM);
	if (this->isTransactionEnabled()) {
		this->db_->get_env()->txn_begin(this->getParentTransaction(), &txn, 0);
	}
	ret = this->db_->put(txn, &k, &data, 0);
	if (this->isTransactionEnabled()) {
		if (ret != 0) {
			txn->abort();
		} else {
			txn->commit(0);
		}
	}
	if (ret != 0) {
		throw DbException(0);
	}
}

void BdbStorage::del(const unsigned char * key, const std::size_t keySize) {
	Dbt k((char*) key, keySize);
	int ret;
	if (this->isTransactionEnabled()) {
		DbTxn * txn;
		this->db_->get_env()->txn_begin(this->getParentTransaction(), &txn, 0);
		ret = this->db_->del(txn, &k, 0);
		if (ret != 0) {
			txn->abort();
		} else {
			txn->commit(0);
		}
	} else {
		ret = this->db_->del(NULL, &k, 0);
	}
	if (ret != 0) {
		throw "Error on deleting root from db";
	}

}
void BdbStorage::clear(void) {
	u_int32_t counter;
	if (this->isTransactionEnabled()) {
		DbTxn * txn;
		this->db_->get_env()->txn_begin(this->getParentTransaction(), &txn, 0);
		try {
			if (this->db_->truncate(txn, &counter, 0) == 0)
				txn->commit(0);
			else
				txn->abort();
		} catch (DbException e) {
			txn->abort();
		}
	} else {
		this->db_->truncate(NULL, &counter, 0);
	}

}
AbstractKeyValueIterator * BdbStorage::createIterator() {
	if (this->isTransactionEnabled()) {
		return new BdbIterator(this->db_, this->getParentTransaction());
	} else {
		return new BdbIterator(this->db_);
	}
}
}

}
