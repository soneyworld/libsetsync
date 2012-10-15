/*
 * DBTrie.cpp
 *
 *      Author: Till Lorentzen
 */

#include "DBTrie.h"
#include "setsync/utils/bitset.h"
#include <stdexcept>
#include <typeinfo>

namespace trie {

const uint8_t DBValue::HAS_PARENT = 0x01;
const uint8_t DBValue::HAS_CHILDREN = 0x02;
const uint8_t DBValue::DIRTY = 0x04;
const std::size_t DBValue::getBufferSize() {
	return 4 * HASHSIZE + 2 * sizeof(uint8_t);
}

void DBValue::marshall(void * target, const DBValue& toBeMarshalled) {
	unsigned char * t = (unsigned char*) target;
	memcpy(t, toBeMarshalled.parent, HASHSIZE);
	memcpy(t + HASHSIZE, toBeMarshalled.smaller, HASHSIZE);
	memcpy(t + 2 * HASHSIZE, toBeMarshalled.larger, HASHSIZE);
	memcpy(t + 3 * HASHSIZE, toBeMarshalled.prefix, HASHSIZE);
	memset(t + 4 * HASHSIZE, toBeMarshalled.prefix_mask, 1);
	memset(t + 4 * HASHSIZE + 1, toBeMarshalled.flags, 1);
}

DBValue::DBValue(const uint8_t flags) {
	memset(this->larger, 0x0, HASHSIZE);
	memset(this->smaller, 0x0, HASHSIZE);
	memset(this->prefix, 0x0, HASHSIZE);
	memset(this->parent, 0x0, HASHSIZE);
	this->prefix_mask = 0;
	this->flags = flags;
}

DBValue::DBValue(const DbNode& toSave) :
	flags(0) {
	if (toSave.hasChildren())
		flags = HAS_CHILDREN;
	if (toSave.hasParent())
		flags = flags | HAS_PARENT;
	if (toSave.isDirty())
		flags = flags | DIRTY;
	memcpy(this->smaller, toSave.smaller, HASHSIZE);
	memcpy(this->larger, toSave.larger, HASHSIZE);
	memcpy(this->prefix, toSave.prefix, HASHSIZE);
	memcpy(this->parent, toSave.parent, HASHSIZE);
	this->prefix_mask = toSave.prefix_mask;
}

DBValue::DBValue(void * toLoad) {
	unsigned char * load = (unsigned char*) toLoad;
	memcpy(this->parent, load, HASHSIZE);
	memcpy(this->smaller, load + HASHSIZE, HASHSIZE);
	memcpy(this->larger, load + 2 * HASHSIZE, HASHSIZE);
	memcpy(this->prefix, load + 3 * HASHSIZE, HASHSIZE);
	this->prefix_mask = (uint8_t) *(load + 4 * HASHSIZE);
	this->flags = (uint8_t) *(load + 4 * HASHSIZE + 1);
}

bool DBValue::isDirty() const {
	return this->flags & DIRTY == DIRTY;
}
bool DBValue::hasChildren() const {
	return this->flags & HAS_CHILDREN == HAS_CHILDREN;
}
bool DBValue::hasParent() const {
	return this->flags & HAS_PARENT == HAS_PARENT;
}

unsigned char DbNode::hashscratch[HASHSIZE * 2];
unsigned char DbNode::nullarray[HASHSIZE];
const char DbRootNode::root_name[] = "root";

DbRootNode::DbRootNode(Db * db) :
	db_(db) {
	Dbt key(const_cast<char *> (root_name), strlen(root_name));
	Dbt data(this->hash, HASHSIZE);
	int ret = this->db_->get(NULL, &key, &data, 0);
	if (ret == DB_NOTFOUND) {
		throw DbTrieException("No Root found");
	} else if (ret != 0) {
		throw DbTrieException("ERROR on loading root");
	}
	memcpy(this->hash, data.get_data(), HASHSIZE);
}

DbRootNode::DbRootNode(Db * db, const unsigned char * hash) :
	db_(db) {
	memcpy(this->hash, hash, HASHSIZE);
	saveToDB();
}

DbNode DbRootNode::getRootNode() {
	return DbNode(this->db_, hash);
}

void DbRootNode::saveToDB(void) {
	Dbt key(const_cast<char*> (root_name), strlen(root_name));
	Dbt data(this->hash, HASHSIZE);
	int ret = this->db_->put(NULL, &key, &data, 0);
	if (ret != 0) {
		throw DbTrieException("Error on putting root to db");
	}
}

DbNode::DbNode(Db * db, const unsigned char * hash, bool newone) :
	db_(db) {
	memcpy(this->hash, hash, HASHSIZE);
	if (newone) {
		this->hasChildren_ = false;
		this->hasParent_ = false;
		memcpy(this->prefix, hash, HASHSIZE);
		this->prefix_mask = 8 * HASHSIZE;
		this->dirty_ = false;
	} else {
		unsigned char result[DBValue::getBufferSize()];
		Dbt key(this->hash, HASHSIZE);
		Dbt data;
		data.set_data(&result);
		data.set_flags(DB_DBT_USERMEM);
		data.set_ulen(DBValue::getBufferSize());
		int ret = this->db_->get(NULL, &key, &data, 0);
		if (ret == 0) {
			DBValue values(data.get_data());
			memcpy(this->parent, values.parent, HASHSIZE);
			memcpy(this->smaller, values.smaller, HASHSIZE);
			memcpy(this->larger, values.larger, HASHSIZE);
			memcpy(this->prefix, values.prefix, HASHSIZE);
			this->dirty_ = values.isDirty();
			this->hasChildren_ = values.hasChildren();
			this->hasParent_ = values.hasParent();
			this->prefix_mask = values.prefix_mask;
		} else {
			throw DbTrieException("DB FAIL");
		}
	}
}

bool DbNode::toDb() {
	unsigned char buffer[DBValue::getBufferSize()];
	DBValue toSave(*this);
	DBValue::marshall(buffer, toSave);
	Dbt key(this->hash, HASHSIZE);
	Dbt data(buffer, DBValue::getBufferSize());
	int ret = this->db_->put(NULL, &key, &data, 0);
	return ret == 0;
}

bool DbNode::similar(const DbNode& node) const {
	for (uint8_t i = 0; i < this->prefix_mask; i++) {
		if (BITTEST(this->prefix, i) != BITTEST(node.prefix,i)) {
			return false;
		}
	}
	return true;
}

uint8_t DbNode::commonPrefixSize(DbNode& other) const {
	uint8_t common = 0;
	for (uint8_t i = 0; i < 8 * HASHSIZE; i++) {
		if (BITTEST(this->prefix, i) != BITTEST(other.prefix, i))
			break;
		common++;
	}
	return common;
}

void DbNode::setChildren(const DbNode& smaller, const DbNode& larger) {
	this->hasChildren_ = true;
	memcpy(this->smaller, smaller.hash, HASHSIZE);
	memcpy(this->larger, larger.hash, HASHSIZE);
	this->dirty_ = true;
}

void DbNode::setParent(const DbNode& parent) {
	this->hasParent_ = true;
	memcpy(this->parent, parent.hash, HASHSIZE);
}

void DbNode::setSmaller(const DbNode& smaller) {
	if (!this->hasChildren_)
		throw DbTrieException("ERROR this node does not have any children");
	memcpy(this->smaller, smaller.hash, HASHSIZE);
	this->dirty_ = true;
}

void DbNode::setLarger(const DbNode& larger) {
	if (!this->hasChildren_)
		throw DbTrieException("ERROR this node does not have any children");
	memcpy(this->larger, larger.hash, HASHSIZE);
	this->dirty_ = true;
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
	bool thesame = similar(node);
	if (thesame) {
		if (node > *this) {
			return this->getLarger().insert(node, performHash);
		} else if (node < *this) {
			return this->getSmaller().insert(node, performHash);
		} else {
			if (this->prefix_mask == 8 * HASHSIZE) {
				// Duplicated key
				return false;
			} else {
				throw DbTrieException("Missing Child!");
			}
		}
	}

	/** Not the same: Need to split */
	// My new common prefix
	uint8_t common = this->commonPrefixSize(node);
	// Copy myself as the new children of myself
	DbNode newchildcopy = DbNode(*this);
	DbNode backup = DbNode(*this);
	if (node > newchildcopy) {
		this->setChildren(newchildcopy,node);
	} else {
		this->setChildren(node,newchildcopy);
	}
	this->updateHash();
	node.setParent(*this);
	newchildcopy.setParent(*this);
	newchildcopy.toDb();
	node.toDb();
	this->prefix_mask = common;
	this->toDb();
	if (performHash) {
		DbNode child = backup;
		while (child.hasParent_) {
			DbNode parent = child.getParent();
			int n = memcmp(parent.larger, child.hash, HASHSIZE);
			if (n == 0) {
				memcpy(parent.larger, child.hash, HASHSIZE);
			} else if(memcmp(parent.smaller, child.hash, HASHSIZE)==0){
				memcpy(parent.smaller, child.hash, HASHSIZE);
			} else {
				throw DbTrieException("father lost!");
			}
			child = parent;
			parent.updateHash();
			parent.toDb();
		}
		DbRootNode(this->db_, child.hash);
	} else {
		if(this->hasParent_){
			DbNode parent = this->getParent();
			parent.dirty_ = true;
			int n = memcmp(parent.larger, newchildcopy.hash, HASHSIZE);
			if (n == 0) {
				memcpy(parent.larger, this->hash, HASHSIZE);
			} else {
				memcpy(parent.smaller, this->hash, HASHSIZE);
			}
			parent.toDb();
		}
	}
	return true;
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
	int n = memcmp(this->hash, other.hash, HASHSIZE);
	if (n != 0)
		return false;
	if (this->hasParent_) {
		n = memcmp(this->parent, other.parent, HASHSIZE);
		if (n != 0)
			return false;
	}
	if (this->dirty_) {
		n = memcmp(this->smaller, other.smaller, HASHSIZE);
		if (n != 0)
			return false;
		n = memcmp(this->larger, other.larger, HASHSIZE);
		if (n != 0)
			return false;
	}
	return true;
}

bool DbNode::operator !=(const DbNode& other) const {
	return !this->operator ==(other);
}

bool DbNode::operator <(const DbNode& other) const {
	if (this->prefix_mask == 8 * HASHSIZE) {
		return false;
	}
	return BITTEST(other.prefix ,this->prefix_mask);
}

bool DbNode::operator >(const DbNode& other) const {
	if (this->prefix_mask == 8 * HASHSIZE) {
		return false;
	}
	return !BITTEST( other.prefix ,this->prefix_mask);
}

DbNode::DbNode(const DbNode& other) :
	dirty_(other.dirty_), hasChildren_(other.hasChildren_),
			hasParent_(other.hasParent_), prefix_mask(other.prefix_mask),
			db_(other.db_) {
	memcpy(this->hash, other.hash, HASHSIZE);
	memcpy(this->parent, other.parent, HASHSIZE);
	memcpy(this->prefix, other.prefix, HASHSIZE);
	memcpy(this->smaller, other.smaller, HASHSIZE);
	memcpy(this->larger, other.larger, HASHSIZE);
}

DbNode& DbNode::operator=(const DbNode& rhs) {
	this->db_ = rhs.db_;
	this->dirty_ = rhs.dirty_;
	this->hasChildren_ = rhs.hasChildren_;
	this->hasParent_ = rhs.hasParent_;
	this->prefix_mask = rhs.prefix_mask;
	memcpy(this->hash, rhs.hash, HASHSIZE);
	memcpy(this->parent, rhs.parent, HASHSIZE);
	memcpy(this->prefix, rhs.prefix, HASHSIZE);
	memcpy(this->smaller, rhs.smaller, HASHSIZE);
	memcpy(this->larger, rhs.larger, HASHSIZE);
	return *this;
}

DbNode DbNode::getSmaller() {
	if (hasChildren_) {
		DbNode result = DbNode(this->db_, this->smaller);
		return result;
	} else {
		throw DbTrieException("THERE ARE NO CHILDREN");
	}
}
DbNode DbNode::getLarger() {
	if (hasChildren_) {
		DbNode result = DbNode(this->db_, this->larger);
		return result;
	} else {
		throw DbTrieException("THERE ARE NO CHILDREN");
	}
}
DbNode DbNode::getParent() {
	if (hasParent_) {
		DbNode result = DbNode(this->db_, this->parent);
		return result;
	} else {
		throw DbTrieException("THERE IS NO PARENT");
	}
}

bool DbNode::insert(const unsigned char * hash, bool performHash) {
	DbNode newnode = DbNode(this->db_, hash, true);
	return this->insert(newnode, performHash);
}

bool DbNode::erase(const unsigned char * hash, bool performHash) {
	//TODO
	return false;
}

void DbNode::updateHash(void) {
	if (hasChildren_) {
		memcpy(DbNode::hashscratch, this->smaller, HASHSIZE);
		memcpy(DbNode::hashscratch + HASHSIZE, this->larger, HASHSIZE);
		SHA1(DbNode::hashscratch, 2 * HASHSIZE, this->hash);
		this->dirty_ = false;
	}
}

bool DbNode::hasParent() const {
	return this->hasParent_;
}

bool DbNode::hasChildren() const {
	return this->hasChildren_;
}

bool DbNode::isDirty() const {
	return this->dirty_;
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

/*bool DBTrie::insertHash(const unsigned char * hash, bool performhash) {
 if (this->root_ == NULL) {
 this->root_ = new DbRootNode(this->db_, hash);
 DbNode rootnode = this->root_->getRootNode();
 return rootnode.toDb();
 } else {
 DbNode root = this->root_->getRootNode();
 return root.insert(hash, performhash);
 }
 }*/

bool DBTrie::add(const unsigned char * hash, bool performhash) {
	if (this->root_ == NULL) {
		this->root_ = new DbRootNode(this->db_, hash);
		DbNode rootnode = this->root_->getRootNode();
		if (rootnode.toDb()) {
			this->incSize();
			return true;
		} else {
			return false;
		}
	} else {
		DbNode root = this->root_->getRootNode();
		if (root.insert(hash, performhash)) {
			this->getRootFromDB();
			this->incSize();
		} else {
			return false;
		}
	}
	/*	unsigned char sha[this->getHashSize()];
	 memcpy(sha, hash, this->getHashSize());
	 Dbt key(sha, this->getHashSize());
	 Dbt data(NULL, 0);
	 int ret = this->db_->put(NULL, &key, &data, DB_NOOVERWRITE);
	 if (ret == DB_KEYEXIST) {
	 return false;
	 } else if (ret == 0) {
	 if (insertHash(hash, performhash)) {
	 this->incSize();
	 return true;
	 } else {
	 return false;
	 }
	 }
	 return false;*/
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
	try {
		if (this->root_ != NULL)
			delete this->root_;
		this->root_ = NULL;
		this->root_ = new DbRootNode(this->db_);
	} catch (...) {

	}
}

void DBTrie::putRootToDB() {
	this->root_->saveToDB();
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
		return this->root_->getRootNode() == other_.root_->getRootNode();
	} catch (const std::bad_cast& e) {
		return false;
	}
}
}
