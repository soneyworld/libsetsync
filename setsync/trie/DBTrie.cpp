/*
 * DBTrie.cpp
 *
 *      Author: Till Lorentzen
 */

#include "DBTrie.h"
#include "setsync/utils/bitset.h"
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

unsigned char DbNode::hashscratch[MAXFIELD * 2];
unsigned char DbNode::nullarray[MAXFIELD];
char DbNode::root_name[] = "root";

DbNode::DbNode(Db * db) :
	db_(db), hasParent_(false) {
	memset(nullarray,0x0,MAXFIELD);
	DbInnerNode root_node;
	Dbt key(root_name, strlen(root_name));
	Dbt data(&root_node, sizeof(DbInnerNode));
	int ret = this->db_->get(NULL, &key, &data, 0);
	if (ret == 0) {
		memcpy(this->hash,root_node.hash, MAXFIELD);
		memset(this->parent,0x0, MAXFIELD);
		memcpy(this->smaller, root_node.value.smaller,MAXFIELD);
		memcpy(this->larger, root_node.value.larger,MAXFIELD);
		int n = memcmp(this->smaller, nullarray,MAXFIELD);
		if(n==0){
			this->hasChildren_ = false;
		}
	} else if(ret == DB_NOTFOUND) {

	}
}

DbNode::DbNode(Db * db, const unsigned char * hash) :
	db_(db) {
	DBValue result;
	memcpy(this->hash, hash, MAXFIELD);
	Dbt key(this->hash, MAXFIELD);
	Dbt data(&result, sizeof(DBValue));
	int ret = this->db_->get(NULL, &key, &data, 0);
	if (ret == 0) {
		memcpy(this->parent,result.parent,MAXFIELD);
		memcpy(this->smaller,result.smaller,MAXFIELD);
		memcpy(this->larger,result.larger,MAXFIELD);
	} else {
		throw "Node nicht gefunden";
	}
}

DbNode DbNode::getNode(const unsigned char * hash){

}

bool DbNode::similar(const DbNode& node) const {
	for (uint8_t i = 0; i < this->prefix_mask; i++) {
		if (BITTEST(this->prefix, i) != BITTEST(this->prefix,i)) {
			return false;
		}
	}
	return true;
}

bool DbNode::insert(DbNode& node) {
	return insert(node, true);
}

bool DbNode::insert(const unsigned char * hash) {
	return insert(hash, true);
}

bool DbNode::erase(DbNode& node, bool performHash) {
	//TODO
	return false;
}

bool DbNode::insert(DbNode& node, bool performHash) {
	//TODO
	return false;
}

bool DbNode::erase(DbNode& node) {
	return erase(node, true);
}

bool DbNode::erase(const unsigned char * hash) {
	return erase(hash, true);
}

bool DbNode::operator ==(const DbNode& other) const {
	if (this->prefix_mask != other.prefix_mask)
		return false;
	if (this->dirty_ != other.dirty_)
		return false;
	if (this->hasChildren_ != other.hasChildren_)
		return false;
	if (this->hasParent_ != other.hasParent_)
		return false;
	int n = memcmp(this->hash, other.hash, MAXFIELD);
	if (n != 0)
		return false;
	if (this->hasParent_) {
		n = memcmp(this->parent, other.parent, MAXFIELD);
		if (n != 0)
			return false;
	}
	if (this->dirty_) {
		n = memcmp(this->smaller, other.smaller, MAXFIELD);
		if (n != 0)
			return false;
		n = memcmp(this->larger, other.larger, MAXFIELD);
		if (n != 0)
			return false;
	}
	return true;
}

bool DbNode::operator !=(const DbNode& other) const {
	return !this->operator ==(other);
}

bool DbNode::operator <(const DbNode& other) const {
	return BITTEST( (unsigned char *)(other.prefix) ,this->prefix_mask);
}

bool DbNode::operator >(const DbNode& other) const {
	return BITTEST( (unsigned char *)(other.prefix) ,this->prefix_mask);
}

DbNode::DbNode(const DbNode& other) :
	dirty_(other.dirty_), hasChildren_(other.hasChildren_),
			hasParent_(other.hasParent_), prefix_mask(other.prefix_mask),
			db_(other.db_) {
	memcpy(this->hash, other.hash, MAXFIELD);
	memcpy(this->parent, other.parent, MAXFIELD);
	memcpy(this->prefix, other.prefix, MAXFIELD);
	memcpy(this->smaller, other.smaller, MAXFIELD);
	memcpy(this->larger, other.larger, MAXFIELD);
}

DbNode& DbNode::operator=(const DbNode& rhs) {
	this->db_ = rhs.db_;
	this->dirty_ = rhs.dirty_;
	this->hasChildren_ = rhs.hasChildren_;
	this->hasParent_ = rhs.hasParent_;
	this->prefix_mask = rhs.prefix_mask;
	memcpy(this->hash, rhs.hash, MAXFIELD);
	memcpy(this->parent, rhs.parent, MAXFIELD);
	memcpy(this->prefix, rhs.prefix, MAXFIELD);
	memcpy(this->smaller, rhs.smaller, MAXFIELD);
	memcpy(this->larger, rhs.larger, MAXFIELD);
	return *this;
}

DbNode DbNode::getSmaller() {
	if (hasChildren_) {
		DbNode result = DbNode(this->db_, this->smaller);
		return result;
	}
}
DbNode DbNode::getLarger() {
	if (hasChildren_) {
		DbNode result = DbNode(this->db_, this->larger);
		return result;
	}
}
DbNode DbNode::getParent() {
	if (hasParent_) {
		DbNode result = DbNode(this->db_, this->parent);
		return result;
	}
}

bool DbNode::insert(const unsigned char * hash, bool performHash) {
	//TODO
	return false;
}

bool DbNode::erase(const unsigned char * hash, bool performHash) {
	//TODO
	return false;
}

void DbNode::updateHash(void) {
	if (hasChildren_) {
		memcpy(DbNode::hashscratch, this->smaller, MAXFIELD);
		memcpy(DbNode::hashscratch + MAXFIELD, this->larger, MAXFIELD);
		SHA1(DbNode::hashscratch, 2 * MAXFIELD, this->hash);
	}
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

bool DBTrie::insertHash(const unsigned char * hash) {
	if (this->root_ == NULL) {
		this->root_ = new DbInnerNode;
		memcpy(this->root_->hash, hash, this->getHashSize());
		memcpy(this->root_->value.prefix, hash, this->getHashSize());
		this->root_->value.prefix_mask = 8 * this->getHashSize();
		putRootToDB();
		return true;
	} else {
		DbInnerNode newnode;
		memcpy(newnode.hash, hash, this->getHashSize());
		memcpy(newnode.value.prefix, hash, this->getHashSize());
		newnode.value.prefix_mask = 8 * this->getHashSize();
		return insertAtNode(hash, this->root_, &newnode);
	}
}

bool DBTrie::insertAtNode(const unsigned char * hash,
		DbInnerNode * currentnode, DbInnerNode * newnode) {
	bool thesame = similar(newnode->value.prefix, currentnode->value.prefix,
			currentnode->value.prefix_mask);
	if (thesame) {
		DbInnerNode next;
		bool
				larger =
						BITTEST( (unsigned char *)(newnode->value.prefix) ,currentnode->value.prefix_mask);
		if (larger)
			if (!loadNode(&next, currentnode->value.larger))
				throw "";
			else if (!loadNode(&next, currentnode->value.smaller))
				throw "";

	}
	return false;
}

bool DBTrie::similar(unsigned char *a, unsigned char *b, uint8_t count) {
	for (uint8_t i = 0; i < count; i++) {
		if (BITTEST(a, i) != BITTEST(b,i)) {
			return false;
		}
	}
	return true;
}

bool DBTrie::loadNode(DbInnerNode * target, const unsigned char * hash) {
	unsigned char sha[this->getHashSize()];
	memcpy(sha, hash, this->getHashSize());
	memcpy(target->hash, hash, this->getHashSize());
	Dbt key(sha, this->getHashSize());
	Dbt data(&target->value, sizeof(DBValue));
	int ret = this->db_->get(NULL, &key, &data, 0);
	if (ret == DB_NOTFOUND) {
		return false;
	} else if (ret == 0) {
		return true;
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
		if (insertHash(hash)) {
			this->incSize();
			return true;
		} else {
			return false;
		}
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
		if (this->getSize() == 0) {
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
		const DBTrie& other_ = dynamic_cast<const DBTrie&> (other);
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
