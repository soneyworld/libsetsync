/*
 * DBTrie.cpp
 *
 *      Author: Till Lorentzen
 */

#include "DBTrie.h"
#include <stdexcept>
#include <typeinfo>


#define ROOT_NAME "root"

namespace trie {

DBValue::DBValue() {
	memset(this->larger, 0x0, MAXFIELD);
	memset(this->smaller, 0x0, MAXFIELD);
	memset(this->prefix, 0x0, MAXFIELD);
	memset(this->parent, 0x0, MAXFIELD);
	this->prefix_mask = 0;
}

DBTrie::DBTrie(Db * db, const size_t hashsize) :
	Trie(hashsize), root_(NULL), db_(db) {
	// Loading root, if available
	getRootFromDB();
}

DBTrie::~DBTrie() {
	if (root_ != NULL && this->db_ != NULL) {
		putRootToDB();
		delete root_;
		root_ = NULL;
	}
}

bool DBTrie::add(const unsigned char * hash, bool performhash) {
	if (this->root_ == NULL) {
		this->root_ = new DbInnerNode;
		memcpy(this->root_->hash, hash, this->getHashSize());
		memcpy(this->root_->value.prefix, hash, this->getHashSize());
		this->root_->value.prefix_mask = 8 * this->getHashSize();
	}
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
		if(this->getSize()==0){
			this->clear();
		}
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
	if (root_ != NULL) {
		delete root_;
		root_ = NULL;
	}

}

void DBTrie::getRootFromDB() {
	char rootname[] = { ROOT_NAME };
	DbInnerNode root_node;
	Dbt key(rootname, strlen(rootname));
	Dbt data(&root_node, sizeof(DbInnerNode));
	int ret = this->db_->get(NULL, &key, &data, 0);
	if (ret != DB_NOTFOUND) {
		this->root_ = new DbInnerNode();
		memcpy(this->root_, &root_node, sizeof(DbInnerNode));
	}
}
void DBTrie::putRootToDB() {
	char rootname[] = { ROOT_NAME };
	DbInnerNode root_node;
	Dbt key(rootname, strlen(rootname));
	Dbt data(&root_node, sizeof(DbInnerNode));
	this->db_->put(NULL, &key, &data, 0);
}

const char * DBTrie::getLogicalDatabaseName() {
	return "trie";
}

const DBTYPE DBTrie::getTableType() {
	return DB_BTREE;
}

bool DBTrie::operator ==(const Trie& other) const {
	try {
		const DBTrie& other_ =
				dynamic_cast<const DBTrie&> (other);
		if (this->root_ != other_.root_)
			return false;
		if (this->root_ == NULL)
			return true;
		int n = memcmp(this->root_->hash, other_.root_->hash,
				this->getHashSize());
		if (n != 0)
			return false;
	} catch (const std::bad_cast& e) {
		return false;
	}
}
}
