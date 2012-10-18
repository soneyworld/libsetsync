/*
 * DBTrie.cpp
 *
 *      Author: Till Lorentzen
 */

#include "DBTrie.h"
#include <setsync/utils/bitset.h>
#include <setsync/utils/OutputFunctions.h>
#include <stdexcept>
#include <typeinfo>
#include <sstream>

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
	return (this->flags & DIRTY) == DIRTY;
}

bool DBValue::hasChildren() const {
	return (this->flags & HAS_CHILDREN) == HAS_CHILDREN;
}

bool DBValue::hasParent() const {
	return (this->flags & HAS_PARENT) == HAS_PARENT;
}

unsigned char DbNode::hashscratch[HASHSIZE * 2];
unsigned char DbNode::nullarray[HASHSIZE];
const char DbRootNode::root_name[] = "root";

DbRootNode::DbRootNode(Db * db) :
	db_(db) {

}

void DbRootNode::set(const unsigned char * hash) {
	Dbt key(const_cast<char*> (root_name), strlen(root_name));
	Dbt data(const_cast<unsigned char*> (hash), HASHSIZE);
	int ret = this->db_->put(NULL, &key, &data, 0);
	if (ret != 0) {
		throw DbTrieException("Error on putting root to db");
	}
}

DbNode DbRootNode::get() const {
	Dbt key(const_cast<char *> (root_name), strlen(root_name));
	unsigned char hash[HASHSIZE];
	Dbt data(hash, HASHSIZE);
	int ret = this->db_->get(NULL, &key, &data, 0);
	if (ret == DB_NOTFOUND) {
		throw DbTrieException("No Root found");
	} else if (ret != 0) {
		throw DbTrieException("ERROR on loading root");
	}
	memcpy(hash, data.get_data(), HASHSIZE);
	return DbNode(this->db_, hash);
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
			throw DbException(ret);
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

bool DbNode::isSmaller(const DbNode& smaller) const {
	if (this->prefix_mask == 8 * HASHSIZE) {
		return false;
	}
	return !BITTEST(smaller.prefix , this->prefix_mask);
}
bool DbNode::isLarger(const DbNode& larger) const {
	if (this->prefix_mask == 8 * HASHSIZE) {
		return false;
	}
	return BITTEST(larger.prefix , this->prefix_mask);
}

bool DbNode::insert(DbNode& node, bool performHash) {
	bool thesame = similar(node);
	if (thesame) {
		if (isLarger(node)) {
			return this->getLarger().insert(node, performHash);
		} else if (isSmaller(node)) {
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
	DbNode intermediate = DbNode(*this);
	// The intermediate common prefix
	intermediate.prefix_mask = intermediate.commonPrefixSize(node);
	// Copy myself as the new children of intermediate
	DbNode oldnode = DbNode(*this);
	if (intermediate < node) {
		intermediate.setChildren(oldnode, node);
	} else {
		intermediate.setChildren(node, oldnode);
	}
	intermediate.updateHash();
	node.setParent(intermediate);
	node.toDb();
	oldnode.setParent(intermediate);
	oldnode.toDb();
	if (!intermediate.hasParent_) {
		intermediate.toDb();
	}
	if (performHash) {
		DbNode oldchild = *this;
		DbNode child = intermediate;
		unsigned char roothash[HASHSIZE];
		memcpy(roothash, intermediate.hash, HASHSIZE);
		while (oldchild.hasParent_) {
			DbNode oldparent = oldchild.getParent();
			DbNode parent = oldparent;
			if (oldparent.isEqualToSmaller(oldchild)) {
				parent.setSmaller(child);
			} else if (oldparent.isEqualToLarger(oldchild)) {
				parent.setLarger(child);
			} else {
				throw DbTrieException(
						"Child hasn't been correct child of parent");
			}
			parent.updateHash();
			child.setParent(parent);
			child.toDb();
			memcpy(roothash, parent.hash, HASHSIZE);
			parent.toDb();
			oldchild = oldparent;
		}
		DbRootNode(this->db_).set(child.hash);
	} else {
		if (intermediate.hasParent_) {
			DbNode parent = intermediate.getParent();
			parent.dirty_ = true;
			if (parent.isEqualToSmaller(oldnode)) {
				parent.setSmaller(intermediate);
			} else if (parent.isEqualToLarger(oldnode)) {
				parent.setLarger(intermediate);
			} else {
				throw DbTrieException("Old Node hasn't been correct child");
			}
			parent.toDb();
		}
		intermediate.toDb();
	}
	return true;
}

bool DbNode::erase(DbNode& node) {
	return erase(node, true);
}

bool DbNode::erase(const unsigned char * hash) {
	return erase(hash, true);
}

bool DbNode::isEqualToSmaller(const DbNode& node) const {
	return memcmp(node.hash, this->smaller, HASHSIZE) == 0;
}
bool DbNode::isEqualToLarger(const DbNode& node) const {
	return memcmp(node.hash, this->larger, HASHSIZE) == 0;
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

DbNode DbNode::getSmaller() const {
	if (hasChildren_) {
		DbNode result = DbNode(this->db_, this->smaller);
		return result;
	} else {
		throw DbTrieException("THERE ARE NO CHILDREN");
	}
}
DbNode DbNode::getLarger() const {
	if (hasChildren_) {
		DbNode result = DbNode(this->db_, this->larger);
		return result;
	} else {
		throw DbTrieException("THERE ARE NO CHILDREN");
	}
}
DbNode DbNode::getParent() const {
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
	return false;
	//TODO
	unsigned char sha[HASHSIZE];
	memcpy(sha, hash, HASHSIZE);
	Dbt key(sha, HASHSIZE);
	Dbt data(NULL, 0);
	int ret = this->db_->del(NULL, &key, 0);
	if (ret == DB_NOTFOUND) {
		return false;
	} else if (ret == 0) {
		return true;
	}
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

bool DbNode::deleteFromDb() {
	Dbt key(this->hash, HASHSIZE);
	return this->db_->del(NULL, &key, 0) == 0;
}

std::string DbNode::toString() const {
	std::stringstream ss;
	ss << "N" << utils::OutputFunctions::CryptoHashtoString(hash) << " [";
	ss << "label=\"{";
	if (hasChildren_)
		ss << "{";
	ss << utils::OutputFunctions::CryptoHashtoString(hash, 6);
	ss << "...}";
	if (hasChildren_) {
		ss << "|{";
		ss << utils::OutputFunctions::CryptoHashtoString(prefix,
				this->prefix_mask / 8);
		ss << "}";
	}
	ss << "\"";
	if (hasChildren_) {
		ss << ", shape=record";
	}
	ss << "];" << std::endl;
	if (hasParent_) {
		ss << "N" << utils::OutputFunctions::CryptoHashtoString(hash) << " -> "
				<< "N" << utils::OutputFunctions::CryptoHashtoString(parent);
		ss << " [style=dotted];" << std::endl;
	}
	if (hasChildren_) {
		ss << "N" << utils::OutputFunctions::CryptoHashtoString(hash) << " -> "
				<< "N" << utils::OutputFunctions::CryptoHashtoString(smaller)
				<< " [label=\"smaller\"];" << std::endl;
		ss << "N" << utils::OutputFunctions::CryptoHashtoString(hash) << " -> "
				<< "N" << utils::OutputFunctions::CryptoHashtoString(larger)
				<< " [label=\"larger\"];" << std::endl;
	}

	if (hasChildren_) {
		DbNode smaller = this->getSmaller();
		ss << smaller.toString();
		DbNode larger = this->getLarger();
		ss << larger.toString();
	}
	return ss.str();
}

std::string DbRootNode::toString() const {
	try {
		std::stringstream ss;
		DbNode n(get());
		ss << "ROOT -> N" << utils::OutputFunctions::CryptoHashtoString(n.hash);
		ss << " [shape=plaintext]";
		ss << ";" << std::endl;
		return ss.str();
	} catch (...) {
		std::stringstream ss;
		ss << "ROOT [shape=plaintext];" << std::endl;
		return ss.str();
	}
}

DBTrie::DBTrie(Db * db, const size_t hashsize) :
	Trie(hashsize), root_(db), db_(db) {
	// Loading root, if available
}

DBTrie::~DBTrie() {
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
	try {
		DbNode root = this->root_.get();
		bool inserted = root.insert(hash, performhash);
		if (inserted)
			incSize();
		return inserted;
	} catch (...) {
		// Create a new node
		DbNode root(this->db_, hash, true);
		root.toDb();
		this->root_.set(hash);
		incSize();
		return true;
	}
}

bool DBTrie::remove(const unsigned char * hash, bool performhash) {
	try {
		DbNode root = this->root_.get();
		bool removed = root.erase(hash, performhash);
		if (removed)
			decSize();
		if (getSize() == 0) {
			this->clear();
		}
		return removed;
	} catch (...) {
		return false;
	}
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

const char * DBTrie::getLogicalDatabaseName() {
	return "trie";
}

const DBTYPE DBTrie::getTableType() {
	return DB_BTREE;
}

bool DBTrie::operator ==(const Trie& other) const {
	try {
		const DBTrie& other_ = dynamic_cast<const DBTrie&> (other);
		try {
			DbNode mynode = root_.get();
		} catch (...) {
			try {
				DbNode othernode = other_.root_.get();
				return true;
			} catch (...) {
				return false;
			}
		}
		return this->root_.get() == other_.root_.get();
	} catch (const std::bad_cast& e) {
		return false;
	}
}

std::string DBTrie::toString() const {
	std::stringstream ss;
	ss << "digraph trie {" << std::endl;
	ss << this->root_.toString();
	DbNode node = this->root_.get();
	ss << node.toString();
	ss << "}" << std::endl;
	return ss.str();
}
}
