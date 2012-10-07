/*
 * DBTrie.cpp
 *
 *      Author: Till Lorentzen
 */

#include "DBTrie.h"

namespace trie {

DBTrie::DBTrie(Db * db, const size_t hashsize) :
	Trie(hashsize) {
	// Instantiate the Db object
	this->db_ = db;
	// Open flags;
	u_int32_t oFlags = DB_CREATE;
	try {
		// Open the database
		db_->open(NULL, //		Transaction pointer
				NULL, //	Database file name
				"trie", //		Optional logical database name
				DB_BTREE, //	Database access method
				oFlags, //		Open flags
				0); //			File mode (using defaults)
		// DbException is not subclassed from std::exception, so
		// need to catch both of these.
	} catch (DbException &e) {
		// Error handling code goes here
		this->db_ = NULL;
		throw e;
	} catch (std::exception &e) {
		// Error handling code goes here
		this->db_ = NULL;
		throw e;
	}

}

DBTrie::~DBTrie() {
	try {
		this->db_->close(0);
//		this->db_->remove(NULL, "trie", 0);
//		this->db_->remove(NULL, NULL, 0);
	} catch (DbException &e) {
		// Error handling code goes here
		throw e;
	} catch (std::exception &e) {
		// Error handling code goes here
		throw e;
	}
}

bool DBTrie::add(const unsigned char * hash, bool performhash) {
	unsigned char sha[this->getHashSize()];
	memcpy(sha, hash, this->getHashSize());
	Dbt key(sha, this->getHashSize());
	Dbt data(NULL, 0);
	int ret = this->db_->put(NULL, &key, &data, DB_NOOVERWRITE);
	if (ret == DB_KEYEXIST) {
		return false;
	} else if (ret == 0) {
		this->incSize();
		return true;
	}
	return false;
}
bool DBTrie::remove(const unsigned char * hash, bool performhash) {
	unsigned char sha[this->getHashSize()];
	memcpy(sha, hash, this->getHashSize());
	Dbt key(sha, this->getHashSize());
	Dbt data(NULL, 0);
	int ret = this->db_->del(NULL, &key, 0);
	if (ret == DB_NOTFOUND) {
		return false;
	} else if (ret == 0) {
		this->decSize();
		return true;
	}
	return false;
}
bool DBTrie::contains(const unsigned char * hash) const {
	unsigned char sha[this->getHashSize()];
	memcpy(sha, hash, this->getHashSize());
	Dbt key(sha, this->getHashSize());
	Dbt data(NULL, 0);
	int ret = this->db_->get(NULL, &key, &data, 0);
	if (ret == DB_NOTFOUND) {
		return false;
	} else if (ret == 0) {
		return true;
	}
	return false;
}
void DBTrie::clear(void) {
	u_int32_t count;
	this->db_->truncate(NULL, &count, 0);
	this->setSize(0);
}

}
