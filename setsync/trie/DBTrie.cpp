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
#include <vector>
#include <stdlib.h>
namespace trie {

const uint8_t DBValue::HAS_PARENT = 0x01;
const uint8_t DBValue::HAS_CHILDREN = 0x02;
const uint8_t DBValue::DIRTY = 0x04;
const std::size_t DBValue::getBufferSize(const std::size_t hashsize) {
	return 4 * hashsize + 2 * sizeof(uint8_t);
}

void DBValue::marshall(void * target) {
	unsigned char * t = (unsigned char*) target;
	memcpy(t, parent, hashsize);
	memcpy(t + hashsize, smaller, hashsize);
	memcpy(t + 2 * hashsize, larger, hashsize);
	memcpy(t + 3 * hashsize, prefix, hashsize);
	memset(t + 4 * hashsize, prefix_mask, 1);
	memset(t + 4 * hashsize + 1, flags, 1);
}

DBValue::DBValue(const std::size_t hashsize, const uint8_t flags) :
	hashsize(hashsize) {
	this->parent = (unsigned char *) malloc(4 * hashsize);
	memset(this->parent, 0x0, hashsize * 4);
	this->prefix_mask = 0;
	this->flags = flags;
}

DBValue::DBValue(const DbNode& toSave) :
	flags(0), hashsize(toSave.hashfunction_.getHashSize()) {
	this->parent = (unsigned char *) malloc(4 * hashsize);
	this->smaller = this->parent + hashsize;
	this->larger = this->smaller + hashsize;
	this->prefix = this->larger + hashsize;
	if (toSave.hasChildren())
		flags = HAS_CHILDREN;
	if (toSave.hasParent())
		flags = flags | HAS_PARENT;
	if (toSave.isDirty())
		flags = flags | DIRTY;
	memcpy(this->smaller, toSave.smaller, hashsize);
	memcpy(this->larger, toSave.larger, hashsize);
	memcpy(this->prefix, toSave.prefix, hashsize);
	memcpy(this->parent, toSave.parent, hashsize);
	this->prefix_mask = toSave.prefix_mask;
}

DBValue::DBValue(void * toLoad, const std::size_t hashsize) : hashsize(hashsize){
	unsigned char * load = (unsigned char*) toLoad;
	this->parent = (unsigned char *) malloc(4 * hashsize);
	memcpy(this->parent, load, hashsize);
	memcpy(this->smaller, load + hashsize, hashsize);
	memcpy(this->larger, load + 2 * hashsize, hashsize);
	memcpy(this->prefix, load + 3 * hashsize, hashsize);
	this->prefix_mask = (uint8_t) *(load + 4 * hashsize);
	this->flags = (uint8_t) *(load + 4 * hashsize + 1);
}

DBValue::~DBValue() {
	free(this->parent);
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

const char DbRootNode::root_name[] = "root";

DbRootNode::DbRootNode(const utils::CryptoHash& hashfunction, Db * db) :
	hashfunction_(hashfunction), db_(db) {

}

DbNode DbRootNode::get() const {
	Dbt key(const_cast<char *> (root_name), strlen(root_name));
	unsigned char hash[hashfunction_.getHashSize()];
	Dbt data(hash, hashfunction_.getHashSize());
	int ret = this->db_->get(NULL, &key, &data, 0);
	if (ret == DB_NOTFOUND) {
		throw DbNoRootFoundException();
	} else if (ret != 0) {
		throw DbException(ret);
	}
	memcpy(hash, data.get_data(), hashfunction_.getHashSize());
	return DbNode(this->hashfunction_, this->db_, hash);
}

void DbRootNode::set(const unsigned char * hash) {
	Dbt key(const_cast<char*> (root_name), strlen(root_name));
	Dbt data(const_cast<unsigned char*> (hash), hashfunction_.getHashSize());
	int ret = this->db_->put(NULL, &key, &data, 0);
	if (ret != 0) {
		throw DbTrieException("Error on putting root to db");
	}
}

void DbRootNode::del() {
	Dbt key(const_cast<char*> (root_name), strlen(root_name));
	int ret = this->db_->del(NULL, &key, 0);
	if (ret != 0) {
		throw DbTrieException("Error on deleting root from db");
	}
}

DbNode::DbNode(const utils::CryptoHash& hashfunction, Db * db,
		const unsigned char * hash, bool newone) :
	db_(db), hashfunction_(hashfunction) {
	this->hash = (unsigned char*) malloc(hashfunction_.getHashSize()*5);
	this->smaller = this->hash + hashfunction_.getHashSize();
	this->larger = this->smaller + hashfunction_.getHashSize();
	this->parent = this->larger + hashfunction_.getHashSize();
	this->prefix = this->parent + hashfunction_.getHashSize();
	memcpy(this->hash, hash, hashfunction_.getHashSize());
	if (newone) {
		this->hasChildren_ = false;
		this->hasParent_ = false;
		memcpy(this->prefix, hash, hashfunction_.getHashSize());
		this->prefix_mask = 8 * hashfunction_.getHashSize();
		this->dirty_ = false;
	} else {
		unsigned char result[DBValue::getBufferSize(hashfunction_.getHashSize())];
		Dbt key(this->hash, hashfunction_.getHashSize());
		Dbt data;
		data.set_data(&result);
		data.set_flags(DB_DBT_USERMEM);
		data.set_ulen(DBValue::getBufferSize(hashfunction_.getHashSize()));
		int ret = this->db_->get(NULL, &key, &data, 0);
		if (ret == 0) {
			DBValue values(data.get_data(), this->hashfunction_.getHashSize());
			memcpy(this->parent, values.parent, hashfunction_.getHashSize());
			memcpy(this->smaller, values.smaller, hashfunction_.getHashSize());
			memcpy(this->larger, values.larger, hashfunction_.getHashSize());
			memcpy(this->prefix, values.prefix, hashfunction_.getHashSize());
			this->dirty_ = values.isDirty();
			this->hasChildren_ = values.hasChildren();
			this->hasParent_ = values.hasParent();
			this->prefix_mask = values.prefix_mask;
		} else {
			free(this->hash);
			throw DbException(ret);
		}
	}
}

bool DbNode::toDb() {
	unsigned char buffer[DBValue::getBufferSize(this->hashfunction_.getHashSize())];
	DBValue toSave(*this);
	toSave.marshall(buffer);
	Dbt key(this->hash, hashfunction_.getHashSize());
	Dbt data(buffer, DBValue::getBufferSize(this->hashfunction_.getHashSize()));
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
	for (uint8_t i = 0; i < 8 * hashfunction_.getHashSize(); i++) {
		if (BITTEST(this->prefix, i) != BITTEST(other.prefix, i))
			break;
		common++;
	}
	return common;
}

void DbNode::setChildren(const DbNode& smaller, const DbNode& larger) {
	this->hasChildren_ = true;
	memcpy(this->smaller, smaller.hash, hashfunction_.getHashSize());
	memcpy(this->larger, larger.hash, hashfunction_.getHashSize());
	this->dirty_ = true;
}

void DbNode::setParent(const DbNode& parent) {
	this->hasParent_ = true;
	memcpy(this->parent, parent.hash, hashfunction_.getHashSize());
}

void DbNode::setSmaller(const DbNode& smaller) {
	if (!this->hasChildren_)
		throw DbTrieException("ERROR this node does not have any children");
	memcpy(this->smaller, smaller.hash, hashfunction_.getHashSize());
	this->dirty_ = true;
}

void DbNode::setLarger(const DbNode& larger) {
	if (!this->hasChildren_)
		throw DbTrieException("ERROR this node does not have any children");
	memcpy(this->larger, larger.hash, hashfunction_.getHashSize());
	this->dirty_ = true;
}

bool DbNode::insert(DbNode& node) {
	return insert(node, true);
}

bool DbNode::insert(const unsigned char * hash) {
	return insert(hash, true);
}

bool DbNode::erase(DbNode& node, bool performHash) {
	return erase(node.hash, performHash);
}

bool DbNode::isSmaller(const DbNode& smaller) const {
	if (this->prefix_mask == 8 * hashfunction_.getHashSize()) {
		return false;
	}
	return !BITTEST(smaller.prefix , this->prefix_mask);
}
bool DbNode::isLarger(const DbNode& larger) const {
	if (this->prefix_mask == 8 * hashfunction_.getHashSize()) {
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
			if (this->prefix_mask == 8 * hashfunction_.getHashSize()) {
				// Duplicated key
				return false;
			} else {
				throw DbTrieException("Missing Child!");
			}
		}
	}
	/** Not the same: Need to split */
	DbNode intermediate = *this;
	// The intermediate common prefix
	intermediate.prefix_mask = intermediate.commonPrefixSize(node);
	// Copy myself as the new children of intermediate
	DbNode oldnode = *this;
	if (intermediate.isLarger(node)) {
		intermediate.setChildren(oldnode, node);
	} else if (intermediate.isSmaller(node)) {
		intermediate.setChildren(node, oldnode);
	} else {
		throw DbTrieException("ERROR");
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
		// Update the parent and their direct children
		// All no more needed nodes of the DB
		std::vector<DbNode> toBeDeleted;
		// Child before change
		DbNode oldchild = *this;
		// Child after change
		DbNode child = intermediate;
		// Root node should show to this after updates
		unsigned char roothash[hashfunction_.getHashSize()];
		memcpy(roothash, intermediate.hash, hashfunction_.getHashSize());
		// The old child have got a parent
		while (oldchild.hasParent_) {
			// Request the parent of the old child
			DbNode oldparent = oldchild.getParent();
			// Copy the old parent to create a new parent
			DbNode parent = oldparent;
			// Request the other child of the old parent, which has to get a new parent
			DbNode otherchild = oldparent.getLarger();
			if (oldparent.isEqualToSmaller(oldchild)) {
				// The old child has been the smaller one, replace it with the new child
				parent.setSmaller(child);
			} else if (oldparent.isEqualToLarger(oldchild)) {
				// The other child of the parent has been the smaller one
				otherchild = oldparent.getSmaller();
				// The old child has been the lager one, replace it with the new child
				parent.setLarger(child);
			} else {
				// the old child hasn't been a child of the old parent. This should never occur.
				throw DbTrieException(
						"Child hasn't been correct child of parent");
			}
			// Create the new father
			parent.updateHash();
			// Set the new parent to the other child
			otherchild.setParent(parent);
			otherchild.toDb();
			// Set the new parent to the new child
			child.setParent(parent);
			child.toDb();
			// Copy the new hash as potentially new root
			if (!parent.hasParent_) {
				memcpy(roothash, parent.hash, hashfunction_.getHashSize());
			}
			// Save the new parent
			parent.toDb();
			// Set the old parent as the old child
			oldchild = oldparent;
			// Set the new parent as new child
			child = parent;
			// delete old parent, if finished
			toBeDeleted.push_back(oldparent);
			//			oldparent.deleteFromDb();
		}
		// Set the new root of the trie
		DbRootNode(this->hashfunction_, this->db_).set(roothash);
		// Now delete all old and replaced parents
		std::vector<DbNode>::const_iterator iter;
		for (iter = toBeDeleted.begin(); iter != toBeDeleted.end(); iter++) {
			DbNode d = *iter;
			d.deleteFromDb();
		}
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
		} else {
			DbRootNode(this->hashfunction_, this->db_).set(intermediate.hash);
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
	return memcmp(node.hash, this->smaller, hashfunction_.getHashSize()) == 0;
}
bool DbNode::isEqualToLarger(const DbNode& node) const {
	return memcmp(node.hash, this->larger, hashfunction_.getHashSize()) == 0;
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
	int n = memcmp(this->hash, other.hash, hashfunction_.getHashSize());
	if (n != 0)
		return false;
	if (this->hasParent_) {
		n = memcmp(this->parent, other.parent, hashfunction_.getHashSize());
		if (n != 0)
			return false;
	}
	if (this->dirty_) {
		n = memcmp(this->smaller, other.smaller, hashfunction_.getHashSize());
		if (n != 0)
			return false;
		n = memcmp(this->larger, other.larger, hashfunction_.getHashSize());
		if (n != 0)
			return false;
	}
	return true;
}

bool DbNode::operator !=(const DbNode& other) const {
	return !this->operator ==(other);
}

bool DbNode::operator <(const DbNode& other) const {
	if (this->prefix_mask == 8 * hashfunction_.getHashSize()) {
		return false;
	}
	return BITTEST(other.prefix ,this->prefix_mask);
}

bool DbNode::operator >(const DbNode& other) const {
	if (this->prefix_mask == 8 * hashfunction_.getHashSize()) {
		return false;
	}
	return !BITTEST( other.prefix ,this->prefix_mask);
}

DbNode::DbNode(const DbNode& other) :
	dirty_(other.dirty_), hasChildren_(other.hasChildren_),
			hasParent_(other.hasParent_), prefix_mask(other.prefix_mask),
			db_(other.db_), hashfunction_(other.hashfunction_) {
	this->hash = (unsigned char*) malloc(hashfunction_.getHashSize()*5);
	this->smaller = this->hash + hashfunction_.getHashSize();
	this->larger = this->smaller + hashfunction_.getHashSize();
	this->parent = this->larger + hashfunction_.getHashSize();
	this->prefix = this->parent + hashfunction_.getHashSize();
	memcpy(this->hash, other.hash, hashfunction_.getHashSize());
	memcpy(this->parent, other.parent, hashfunction_.getHashSize());
	memcpy(this->prefix, other.prefix, hashfunction_.getHashSize());
	memcpy(this->smaller, other.smaller, hashfunction_.getHashSize());
	memcpy(this->larger, other.larger, hashfunction_.getHashSize());
}

DbNode& DbNode::operator=(const DbNode& rhs) {
	this->db_ = rhs.db_;
	this->dirty_ = rhs.dirty_;
	this->hasChildren_ = rhs.hasChildren_;
	this->hasParent_ = rhs.hasParent_;
	this->prefix_mask = rhs.prefix_mask;
	this->hash = (unsigned char*) malloc(hashfunction_.getHashSize()*5);
	this->smaller = this->hash + hashfunction_.getHashSize();
	this->larger = this->smaller + hashfunction_.getHashSize();
	this->parent = this->larger + hashfunction_.getHashSize();
	this->prefix = this->parent + hashfunction_.getHashSize();
	memcpy(this->hash, rhs.hash, hashfunction_.getHashSize());
	memcpy(this->parent, rhs.parent, hashfunction_.getHashSize());
	memcpy(this->prefix, rhs.prefix, hashfunction_.getHashSize());
	memcpy(this->smaller, rhs.smaller, hashfunction_.getHashSize());
	memcpy(this->larger, rhs.larger, hashfunction_.getHashSize());
	return *this;
}

DbNode DbNode::getSmaller() const {
	if (hasChildren_) {
		DbNode result = DbNode(this->hashfunction_, this->db_, this->smaller);
		return result;
	} else {
		throw DbTrieException("THERE ARE NO CHILDREN");
	}
}
DbNode DbNode::getLarger() const {
	if (hasChildren_) {
		DbNode result = DbNode(this->hashfunction_, this->db_, this->larger);
		return result;
	} else {
		throw DbTrieException("THERE ARE NO CHILDREN");
	}
}
DbNode DbNode::getParent() const {
	if (hasParent_) {
		DbNode result = DbNode(this->hashfunction_, this->db_, this->parent);
		return result;
	} else {
		throw DbTrieException("THERE IS NO PARENT");
	}
}

bool DbNode::insert(const unsigned char * hash, bool performHash) {
	DbNode newnode = DbNode(this->hashfunction_, this->db_, hash, true);
	return this->insert(newnode, performHash);
}

bool DbNode::erase(const unsigned char * hash, bool performHash) {
	try {
		DbNode toBeDeleted(this->hashfunction_, this->db_, hash);
		if (toBeDeleted.hasChildren_)
			throw DbTrieException("This node is not a leaf");
		if (!toBeDeleted.hasParent_) {
			// Root node should be deleted
			DbRootNode root(this->hashfunction_, this->db_);
			root.del();
			toBeDeleted.deleteFromDb();
		} else {
			DbNode parent = toBeDeleted.getParent();
			DbRootNode root(this->hashfunction_, this->db_);
			if (!parent.hasParent_) {
				// Other Child has to become the new root
				DbNode childOfParent(*this);
				if (parent.isEqualToLarger(toBeDeleted)) {
					childOfParent = parent.getSmaller();
				} else {
					childOfParent = parent.getLarger();
				}
				childOfParent.hasParent_ = false;
				childOfParent.toDb();
				root.set(childOfParent.hash);
				parent.deleteFromDb();
				toBeDeleted.deleteFromDb();
			} else {
				// found node and his parent has to be deleted.
				// His grandparent has to be updated.
				DbNode oldgrandparent = parent.getParent();
				DbNode newgrandparent(oldgrandparent);
				DbNode childOfParent(*this);
				// All old nodes will be deleted after successful execution
				std::vector<DbNode> oldnodes;
				if (parent.isEqualToLarger(toBeDeleted)) {
					childOfParent = parent.getSmaller();
				} else if (parent.isEqualToSmaller(toBeDeleted)) {
					childOfParent = parent.getLarger();
				} else {
					throw DbTrieException("Parent has wrong children");
				}
				DbNode childOfGrandParent(*this);
				if (oldgrandparent.isEqualToLarger(parent)) {
					childOfGrandParent = oldgrandparent.getSmaller();
					newgrandparent.setLarger(childOfParent);
				} else if (oldgrandparent.isEqualToSmaller(parent)) {
					childOfGrandParent = oldgrandparent.getLarger();
					newgrandparent.setSmaller(childOfParent);
				} else {
					throw DbTrieException("Grandparent has wrong children");
				}
				newgrandparent.updateHash();
				childOfParent.setParent(newgrandparent);
				childOfGrandParent.setParent(newgrandparent);
				newgrandparent.toDb();
				childOfParent.toDb();
				childOfGrandParent.toDb();
				oldnodes.push_back(parent);
				oldnodes.push_back(toBeDeleted);
				if (!oldgrandparent.hasParent_) {
					oldnodes.push_back(oldgrandparent);
					root.set(newgrandparent.hash);
				} else {
					if (performHash) {
						oldnodes.push_back(oldgrandparent);
						while (oldgrandparent.hasParent_) {
							DbNode oldgrandgrandparent =
									oldgrandparent.getParent();
							DbNode newgrandgrandparent(oldgrandgrandparent);
							DbNode otherchild(*this);
							if (oldgrandgrandparent.isEqualToLarger(
									oldgrandparent)) {
								newgrandgrandparent.setLarger(newgrandparent);
								otherchild = oldgrandgrandparent.getSmaller();
							} else if (oldgrandgrandparent.isEqualToSmaller(
									oldgrandparent)) {
								newgrandgrandparent.setSmaller(newgrandparent);
								otherchild = oldgrandgrandparent.getLarger();
							} else {
								throw DbTrieException(
										"a old grandparent hasn't been child of a old grandgrandparent");
							}
							newgrandgrandparent.updateHash();
							newgrandgrandparent.toDb();
							newgrandparent.setParent(newgrandgrandparent);
							newgrandparent.toDb();
							otherchild.setParent(newgrandgrandparent);
							otherchild.toDb();
							oldnodes.push_back(oldgrandgrandparent);
							oldgrandparent = oldgrandgrandparent;
							newgrandparent = newgrandgrandparent;
						}
						root.set(newgrandparent.hash);
					} else {
						DbNode oldgrandgrandparent = oldgrandparent.getParent();
						if (oldgrandgrandparent.isEqualToLarger(oldgrandparent)) {
							oldgrandgrandparent.setLarger(newgrandparent);
						} else if (oldgrandgrandparent.isEqualToSmaller(
								oldgrandparent)) {
							oldgrandgrandparent.setSmaller(newgrandparent);
						} else {
							throw DbTrieException(
									"grandparent hasn't been child of grandgrandparent");
						}
						oldnodes.push_back(oldgrandparent);
						oldgrandgrandparent.toDb();
					}
				}

				std::vector<DbNode>::const_iterator iter;
				for (iter = oldnodes.begin(); iter != oldnodes.end(); iter++) {
					DbNode d = *iter;
					d.deleteFromDb();
				}
			}
		}
	} catch (DbException e) {
		if (e.get_errno() == DB_NOTFOUND)
			return false;
		else
			throw e;
	}
	return true;
}

void DbNode::updateHash(void) {
	if (hasChildren_) {
		memcpy(DbNode::hashscratch, this->smaller,
				this->hashfunction_.getHashSize());
		memcpy(DbNode::hashscratch + this->hashfunction_.getHashSize(),
				this->larger, this->hashfunction_.getHashSize());
		this->hashfunction_(this->hash, DbNode::hashscratch,
				2 * this->hashfunction_.getHashSize());
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
	Dbt key(this->hash, this->hashfunction_.getHashSize());
	return this->db_->del(NULL, &key, 0) == 0;
}

std::string DbNode::toString() const {
	std::stringstream ss;
	ss << "N" << utils::OutputFunctions::CryptoHashtoString(hash) << " [";
	ss << "label=\"";
	if (hasChildren_)
		ss << "{{";
	ss << "hash: 0x" << utils::OutputFunctions::CryptoHashtoString(hash, 3);
	ss << "...";
	if (!hasChildren_) {
		ss << "\\n0b" << utils::OutputFunctions::ArrayToBitString(hash, 8)
				<< "...";
	}
	if (hasChildren_) {
		ss << "}|{prefix(" << (int) this->prefix_mask << "): ";
		ss << utils::OutputFunctions::ArrayToBitString(prefix,
				this->prefix_mask);
		ss << "}}";
	}
	ss << "\"";
	if (hasChildren_) {
		ss << ", shape=record";
	}
	if (dirty_) {
		ss << ", color=red";
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
	std::string result = ss.str();
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

DBTrie::DBTrie(const utils::CryptoHash& hash, Db * db) :
	Trie(hash), root_(hash, db), db_(db) {
	// Loading root, if available
	this->hashscratch = (unsigned char* ) malloc(this->hash_.getHashSize()*2);
}

DBTrie::~DBTrie() {
	free(this->hashscratch);
}

bool DBTrie::add(const unsigned char * hash, bool performhash) {
	try {
		DbNode root = this->root_.get();
		bool inserted = root.insert(hash, performhash);
		if (inserted)
			incSize();
		return inserted;
	} catch (...) {
		// Create a new node
		DbNode root(this->hash_, this->db_, hash, true);
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
	unsigned char k[this->hash_.getHashSize()];
	memcpy(k, hash, this->hash_.getHashSize());
	Dbt key(k, this->hash_.getHashSize());
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
	return DB_HASH;
}

bool DBTrie::operator ==(const Trie& other) const {
	try {
		const DBTrie& other_ = dynamic_cast<const DBTrie&> (other);
		try {
			root_.get();
		} catch (DbNoRootFoundException e) {
			try {
				other_.root_.get();
				return false;
			} catch (DbNoRootFoundException e) {
				return true;
			}
		}
		try {
			other_.root_.get();
		} catch (DbNoRootFoundException e) {
			return false;
		}
		DbNode mynode = root_.get();
		DbNode othernode = other_.root_.get();
		return mynode == othernode;
	} catch (const std::bad_cast& e) {
		return false;
	}
}

std::string DBTrie::toString() const {
	std::stringstream ss;
	ss << "digraph trie {" << std::endl;
	ss << this->root_.toString();
	try {
		DbNode node = this->root_.get();
		ss << node.toString();
	} catch (DbTrieException e) {

	}
	ss << "}" << std::endl;
	return ss.str();
}
}
