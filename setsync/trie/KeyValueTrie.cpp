/*
 * KeyValueTrie.cpp
 *
 *      Author: Till Lorentzen
 */

#include "KeyValueTrie.h"
#include <sstream>
#include <stdexcept>
#include <typeinfo>
#include <setsync/utils/bitset.h>
#include <setsync/utils/OutputFunctions.h>
#include <setsync/trie/TrieException.h>
#include <stdlib.h>

namespace trie {

const uint8_t TrieNode::HAS_PARENT = 0x01;
const uint8_t TrieNode::HAS_CHILDREN = 0x02;
const uint8_t TrieNode::DIRTY = 0x04;

std::size_t TrieNode::getMarshallBufferSize(const TrieNode& node) {
	return 4 * node.hashfunction_.getHashSize() + 2 * sizeof(uint8_t);
}

void TrieNode::marshall(const TrieNode& source,
		const unsigned char * targetBuffer) {
	unsigned char * t = (unsigned char*) targetBuffer;
	size_t hashsize = source.hashfunction_.getHashSize();
	memcpy(t, source.parent, hashsize);
	memcpy(t + hashsize, source.smaller, hashsize);
	memcpy(t + 2 * hashsize, source.larger, hashsize);
	memcpy(t + 3 * hashsize, source.prefix, hashsize);
	memset(t + 4 * hashsize, source.prefix_mask, 1);
	memset(t + 4 * hashsize + 1, source.getFlags(), 1);
}

const char KeyValueRootNode::root_name[] = "root";

KeyValueRootNode::KeyValueRootNode(const KeyValueTrie& trie,
		const utils::CryptoHash& hashfunction,
		setsync::storage::AbstractKeyValueStorage& storage) :
	storage_(storage), hashfunction_(hashfunction), trie_(trie) {
}

TrieNode KeyValueRootNode::get() const {
	unsigned char * rootHash;
	size_t resultSize;
	if (this->storage_.get((unsigned char *) root_name, strlen(root_name),
			&rootHash, &resultSize)) {
		if (resultSize != hashfunction_.getHashSize()) {
			throw TrieException("wrong root key size");
		}
		TrieNode rootNode = TrieNode(this->trie_, this->hashfunction_,
				this->storage_, rootHash);
		free(rootHash);
		return rootNode;
	} else {
		throw TrieRootNotFoundException();
	}
}

void KeyValueRootNode::set(const unsigned char * hash) {
	this->storage_.put((unsigned char *) root_name, strlen(root_name), hash,
			hashfunction_.getHashSize());
}

void KeyValueRootNode::del() {
	this->storage_.del((unsigned char *) root_name, strlen(root_name));
}

std::string KeyValueRootNode::toString(const std::string nodePrefix) const {
	try {
		std::stringstream ss;
		TrieNode n(get());
		ss << nodePrefix << "ROOT [label=\"ROOT\"];" << std::endl;
		ss << nodePrefix << "ROOT -> " << nodePrefix
				<< utils::OutputFunctions::CryptoHashtoString(n.hash);
		ss << " [shape=plaintext];" << std::endl;
		return ss.str();
	} catch (...) {
		std::stringstream ss;
		ss << nodePrefix << "ROOT [label=\"ROOT\";shape=plaintext];"
				<< std::endl;
		return ss.str();
	}
}

TrieNode::TrieNode(const KeyValueTrie& trie,
		const utils::CryptoHash& hashfunction,
		setsync::storage::AbstractKeyValueStorage& storage,
		const unsigned char * hash, bool newone) :
	storage_(storage), hashfunction_(hashfunction), trie_(trie) {
	this->hash = (unsigned char*) malloc(hashfunction_.getHashSize() * 5);
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
		unsigned char * result;
		size_t resultSize;
		if (this->storage_.get(hash, hashfunction_.getHashSize(), &result,
				&resultSize)) {
			unmarshall(*this, result, resultSize);
			free(result);
		} else {
			free(this->hash);
			throw TrieNodeNotFoundException();
		}
	}
}

TrieNode::~TrieNode() {
	free(this->hash);
}

void TrieNode::unmarshall(TrieNode& target, const unsigned char * loadedValue,
		const std::size_t loadedSize) {
	size_t hashsize = target.hashfunction_.getHashSize();
	memcpy(target.parent, loadedValue, hashsize);
	memcpy(target.smaller, loadedValue + hashsize, hashsize);
	memcpy(target.larger, loadedValue + 2 * hashsize, hashsize);
	memcpy(target.prefix, loadedValue + 3 * hashsize, hashsize);
	target.prefix_mask = (uint8_t) *(loadedValue + 4 * hashsize);
	uint8_t flags = (uint8_t) *(loadedValue + 4 * hashsize + 1);
	target.dirty_ = (flags & DIRTY) == DIRTY;
	target.hasChildren_ = (flags & HAS_CHILDREN) == HAS_CHILDREN;
	target.hasParent_ = (flags & HAS_PARENT) == HAS_PARENT;
	_unused(loadedSize);
}

bool TrieNode::toDb() {
	size_t hashsize = this->hashfunction_.getHashSize();
	unsigned char buffer[getMarshallBufferSize(*this)];
	marshall(*this, buffer);
	try {
		this ->storage_.put(this->hash, hashsize, buffer,
				getMarshallBufferSize(*this));
	} catch (KeyValueStoreException e) {
		return false;
	}
	return true;
}

bool TrieNode::similar(const TrieNode& node) const {
	for (uint8_t i = 0; i < this->prefix_mask; i++) {
		if (BITTEST(this->prefix, i) != BITTEST(node.prefix, i)) {
			return false;
		}
	}
	return true;
}

uint8_t TrieNode::commonPrefixSize(const TrieNode& other) const {
	uint8_t common = 0;
	for (uint8_t i = 0; i < 8 * hashfunction_.getHashSize(); i++) {
		if (BITTEST(this->prefix, i) != BITTEST(other.prefix, i))
			break;
		common++;
	}
	return common;
}

void TrieNode::setChildren(const TrieNode& smaller, const TrieNode& larger) {
	this->hasChildren_ = true;
	memcpy(this->smaller, smaller.hash, hashfunction_.getHashSize());
	memcpy(this->larger, larger.hash, hashfunction_.getHashSize());
	this->dirty_ = true;
}

void TrieNode::setParent(const TrieNode& parent) {
	this->hasParent_ = true;
	memcpy(this->parent, parent.hash, hashfunction_.getHashSize());
}

void TrieNode::setSmaller(const TrieNode& smaller) {
	if (!this->hasChildren_)
		throw DbTrieException("ERROR this node does not have any children");
	memcpy(this->smaller, smaller.hash, hashfunction_.getHashSize());
	this->dirty_ = true;
}

void TrieNode::setLarger(const TrieNode& larger) {
	if (!this->hasChildren_)
		throw DbTrieException("ERROR this node does not have any children");
	memcpy(this->larger, larger.hash, hashfunction_.getHashSize());
	this->dirty_ = true;
}

bool TrieNode::insert(TrieNode& node) {
	return insert(node, true);
}

bool TrieNode::insert(const unsigned char * hash) {
	return insert(hash, true);
}

bool TrieNode::erase(TrieNode& node, bool performHash) {
	return erase(node.hash, performHash);
}

bool TrieNode::isSmaller(const TrieNode& smaller) const {
	if (this->prefix_mask == 8 * hashfunction_.getHashSize()) {
		return false;
	}
	return !BITTEST(smaller.prefix, this->prefix_mask);
}
bool TrieNode::isLarger(const TrieNode& larger) const {
	if (this->prefix_mask == 8 * hashfunction_.getHashSize()) {
		return false;
	}
	return BITTEST(larger.prefix, this->prefix_mask);
}

bool TrieNode::insert(TrieNode& node, bool performHash) {
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
	TrieNode intermediate = *this;
	// The intermediate common prefix
	intermediate.prefix_mask = intermediate.commonPrefixSize(node);
	// Copy myself as the new children of intermediate
	TrieNode oldnode = *this;
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
		std::vector<TrieNode> toBeDeleted;
		// Child before change
		TrieNode oldchild = *this;
		// Child after change
		TrieNode child = intermediate;
		// Root node should show to this after updates
		unsigned char roothash[hashfunction_.getHashSize()];
		memcpy(roothash, intermediate.hash, hashfunction_.getHashSize());
		// The old child have got a parent
		while (oldchild.hasParent_) {
			// Request the parent of the old child
			TrieNode oldparent = oldchild.getParent();
			// Copy the old parent to create a new parent
			TrieNode parent = oldparent;
			// Request the other child of the old parent, which has to get a new parent
			TrieNode otherchild = oldparent.getLarger();
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
		this->trie_.root_->set(roothash);
		// Now delete all old and replaced parents
		std::vector<TrieNode>::const_iterator iter;
		for (iter = toBeDeleted.begin(); iter != toBeDeleted.end(); iter++) {
			TrieNode d = *iter;
			d.deleteFromDb();
		}
	} else {
		if (intermediate.hasParent_) {
			TrieNode parent = intermediate.getParent();
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
			this->trie_.root_->set(intermediate.hash);
		}
		intermediate.toDb();
	}
	return true;
}

bool TrieNode::erase(TrieNode& node) {
	return erase(node, true);
}

bool TrieNode::erase(const unsigned char * hash) {
	return erase(hash, true);
}

bool TrieNode::isEqualToSmaller(const TrieNode& node) const {
	return memcmp(node.hash, this->smaller, hashfunction_.getHashSize()) == 0;
}
bool TrieNode::isEqualToLarger(const TrieNode& node) const {
	return memcmp(node.hash, this->larger, hashfunction_.getHashSize()) == 0;
}

bool TrieNode::operator ==(const TrieNode& other) const {
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

bool TrieNode::operator !=(const TrieNode& other) const {
	return !this->operator ==(other);
}

bool TrieNode::operator <(const TrieNode& other) const {
	if (this->prefix_mask == 8 * hashfunction_.getHashSize()) {
		return false;
	}
	return BITTEST(other.prefix, this->prefix_mask);
}

bool TrieNode::operator >(const TrieNode& other) const {
	if (this->prefix_mask == 8 * hashfunction_.getHashSize()) {
		return false;
	}
	return !BITTEST(other.prefix, this->prefix_mask);
}

TrieNode::TrieNode(const TrieNode& other) :
	storage_(other.storage_), hasChildren_(other.hasChildren_),
			hasParent_(other.hasParent_), prefix_mask(other.prefix_mask),
			dirty_(other.dirty_), hashfunction_(other.hashfunction_),
			trie_(other.trie_) {
	this->hash = (unsigned char*) malloc(hashfunction_.getHashSize() * 5);
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

TrieNode& TrieNode::operator=(const TrieNode& rhs) {
	this->storage_ = rhs.storage_;
	this->dirty_ = rhs.dirty_;
	this->hasChildren_ = rhs.hasChildren_;
	this->hasParent_ = rhs.hasParent_;
	this->prefix_mask = rhs.prefix_mask;
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

TrieNode TrieNode::getSmaller() const {
	if (hasChildren_) {
		TrieNode result = TrieNode(this->trie_, this->hashfunction_,
				this->storage_, this->smaller);
		return result;
	} else {
		throw DbTrieException("THERE ARE NO CHILDREN");
	}
}
TrieNode TrieNode::getLarger() const {
	if (hasChildren_) {
		TrieNode result = TrieNode(this->trie_, this->hashfunction_,
				this->storage_, this->larger);
		return result;
	} else {
		throw DbTrieException("THERE ARE NO CHILDREN");
	}
}
TrieNode TrieNode::getParent() const {
	if (hasParent_) {
		TrieNode result = TrieNode(this->trie_, this->hashfunction_,
				this->storage_, this->parent);
		return result;
	} else {
		throw DbTrieException("THERE IS NO PARENT");
	}
}

bool TrieNode::insert(const unsigned char * hash, bool performHash) {
	TrieNode newnode = TrieNode(this->trie_, this->hashfunction_,
			this->storage_, hash, true);
	return this->insert(newnode, performHash);
}

bool TrieNode::erase(const unsigned char * hash, bool performHash) {
	try {
		TrieNode toBeDeleted(this->trie_, this->hashfunction_, this->storage_,
				hash);
		if (toBeDeleted.hasChildren_)
			throw DbTrieException("This node is not a leaf");
		if (!toBeDeleted.hasParent_) {
			// Root node should be deleted
			this->trie_.root_->del();
			toBeDeleted.deleteFromDb();
		} else {
			TrieNode parent = toBeDeleted.getParent();
			if (!parent.hasParent_) {
				// Other Child has to become the new root
				TrieNode childOfParent(*this);
				if (parent.isEqualToLarger(toBeDeleted)) {
					childOfParent = parent.getSmaller();
				} else {
					childOfParent = parent.getLarger();
				}
				childOfParent.hasParent_ = false;
				childOfParent.toDb();
				this->trie_.root_->set(childOfParent.hash);
				parent.deleteFromDb();
				toBeDeleted.deleteFromDb();
			} else {
				// found node and his parent has to be deleted.
				// His grandparent has to be updated.
				TrieNode oldgrandparent = parent.getParent();
				TrieNode newgrandparent(oldgrandparent);
				TrieNode childOfParent(*this);
				// All old nodes will be deleted after successful execution
				std::vector<TrieNode> oldnodes;
				if (parent.isEqualToLarger(toBeDeleted)) {
					childOfParent = parent.getSmaller();
				} else if (parent.isEqualToSmaller(toBeDeleted)) {
					childOfParent = parent.getLarger();
				} else {
					throw DbTrieException("Parent has wrong children");
				}
				TrieNode childOfGrandParent(*this);
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
					this->trie_.root_->set(newgrandparent.hash);
				} else {
					if (performHash) {
						oldnodes.push_back(oldgrandparent);
						while (oldgrandparent.hasParent_) {
							TrieNode oldgrandgrandparent =
									oldgrandparent.getParent();
							TrieNode newgrandgrandparent(oldgrandgrandparent);
							TrieNode otherchild(*this);
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
						this->trie_.root_->set(newgrandparent.hash);
					} else {
						TrieNode oldgrandgrandparent =
								oldgrandparent.getParent();
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

				std::vector<TrieNode>::const_iterator iter;
				for (iter = oldnodes.begin(); iter != oldnodes.end(); iter++) {
					TrieNode d = *iter;
					d.deleteFromDb();
				}
			}
		}
	} catch (TrieNodeNotFoundException e) {
		return false;
	}
	return true;
}

void TrieNode::updateHash(void) {
	if (hasChildren_) {
		memcpy(trie_.hashscratch, this->smaller,
				this->hashfunction_.getHashSize());
		memcpy(trie_.hashscratch + this->hashfunction_.getHashSize(),
				this->larger, this->hashfunction_.getHashSize());
		this->hashfunction_(this->hash, trie_.hashscratch,
				2 * this->hashfunction_.getHashSize());
		this->dirty_ = false;
	}
}

bool TrieNode::hasParent() const {
	return this->hasParent_;
}

bool TrieNode::hasChildren() const {
	return this->hasChildren_;
}

bool TrieNode::isDirty() const {
	return this->dirty_;
}

bool TrieNode::deleteFromDb() {
	try {
		this->storage_.del(this->hash, hashfunction_.getHashSize());
		return true;
	} catch (KeyValueStoreException e) {
		return false;
	}

}

std::string TrieNode::toString(const std::string nodePrefix) const {
	std::stringstream ss;
	ss << nodePrefix << utils::OutputFunctions::CryptoHashtoString(hash)
			<< " [";
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
		ss << nodePrefix << utils::OutputFunctions::CryptoHashtoString(hash)
				<< " -> " << nodePrefix
				<< utils::OutputFunctions::CryptoHashtoString(parent);
		ss << " [style=dotted];" << std::endl;
	}
	if (hasChildren_) {
		ss << nodePrefix << utils::OutputFunctions::CryptoHashtoString(hash)
				<< " -> " << nodePrefix
				<< utils::OutputFunctions::CryptoHashtoString(smaller)
				<< " [label=\"smaller\"];" << std::endl;
		ss << nodePrefix << utils::OutputFunctions::CryptoHashtoString(hash)
				<< " -> " << nodePrefix
				<< utils::OutputFunctions::CryptoHashtoString(larger)
				<< " [label=\"larger\"];" << std::endl;
	}
	std::string result = ss.str();
	if (hasChildren_) {
		TrieNode smaller = this->getSmaller();
		ss << smaller.toString(nodePrefix);
		TrieNode larger = this->getLarger();
		ss << larger.toString(nodePrefix);
	}
	return ss.str();
}

uint8_t TrieNode::getFlags() const {
	uint8_t flags = 0;
	if (this->hasChildren_)
		flags = HAS_CHILDREN;
	if (this->hasParent_)
		flags = flags | HAS_PARENT;
	if (this->dirty_)
		flags = flags | DIRTY;
	return flags;
}

KeyValueTrie::KeyValueTrie(const utils::CryptoHash& hash,
		setsync::storage::AbstractKeyValueStorage& storage) :
	Trie(hash), storage_(storage) {
	this->root_ = new KeyValueRootNode(*this, hash, storage_);
}

KeyValueTrie::~KeyValueTrie() {
	delete this->root_;
}

bool KeyValueTrie::add(const unsigned char * hash, bool performhash) {
	try {
		TrieNode root = this->root_->get();
		bool inserted = root.insert(hash, performhash);
		if (inserted)
			incSize();
		return inserted;
	} catch (...) {
		// Create a new node
		TrieNode root(*this, this->hash_, this->storage_, hash, true);
		root.toDb();
		this->root_->set(hash);
		incSize();
		return true;
	}
}

bool KeyValueTrie::remove(const unsigned char * hash, bool performhash) {
	try {
		TrieNode root = this->root_->get();
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

TrieNodeType KeyValueTrie::contains(const unsigned char * hash) const {
	try {
		TrieNode node(*this, this->hash_, this->storage_, hash, false);
		if (node.hasChildren_) {
			return INNER_NODE;
		} else {
			return LEAF_NODE;
		}
	} catch (...) {
		return NOT_FOUND;
	}
	return NOT_FOUND;
}

void KeyValueTrie::clear(void) {
	this->storage_.clear();
	this->setSize(0);
}

bool KeyValueTrie::operator ==(const Trie& other) const {
	try {
		const KeyValueTrie& other_ = dynamic_cast<const KeyValueTrie&> (other);
		try {
			root_->get();
		} catch (TrieRootNotFoundException e) {
			try {
				other_.root_->get();
				return false;
			} catch (TrieRootNotFoundException e) {
				return true;
			}
		}
		try {
			other_.root_->get();
		} catch (TrieRootNotFoundException e) {
			return false;
		}
		TrieNode mynode = root_->get();
		TrieNode othernode = other_.root_->get();
		return mynode == othernode;
	} catch (const std::bad_cast& e) {
		return false;
	}
}

std::string KeyValueTrie::toDotString(const std::string nodePrefix) const {
	std::stringstream ss;
	ss << this->root_->toString(nodePrefix);
	try {
		TrieNode node = this->root_->get();
		ss << node.toString(nodePrefix);
	} catch (DbTrieException e) {

	}
	return ss.str();
}

std::string KeyValueTrie::toString() const {
	std::stringstream ss;
	ss << "digraph trie {" << std::endl;
	ss << this->Trie::toDotString();
	ss << "}" << std::endl;
	return ss.str();
}

size_t KeyValueTrie::getSubTrie(const TrieNode& root,
		const size_t numberOfNodes, std::vector<TrieNode>& inner_nodes,
		std::vector<TrieNode>& child_nodes) {
	if (root.hasChildren_ && numberOfNodes >= 2) {
		TrieNode smaller = root.getSmaller();
		TrieNode larger = root.getLarger();
		size_t smallercount = numberOfNodes / 2;
		smallercount = getSubTrie(smaller, smallercount, inner_nodes,
				child_nodes);
		size_t largercount = numberOfNodes - smallercount;
		largercount = getSubTrie(larger, largercount, inner_nodes, child_nodes);
		size_t sum = smallercount + largercount;
		while (sum < numberOfNodes && inner_nodes.size() > 0) {
			TrieNode innerNode = inner_nodes.back();
			inner_nodes.pop_back();
			sum--;
			sum += getSubTrie(innerNode, numberOfNodes - sum, inner_nodes,
					child_nodes);
		}
		return sum;
	} else {
		if (root.hasChildren_) {
			inner_nodes.push_back(root);
		} else {
			child_nodes.push_back(root);
		}
		return 1;
	}
}

size_t KeyValueTrie::getSubTrie(const unsigned char * hash, void * buffer,
		const size_t buffersize) {
	size_t maxNumberOfHashes = buffersize / this->hash_.getHashSize();
	if (maxNumberOfHashes < 2) {
		throw "buffer is too small!";
	}
	std::vector<TrieNode> inner_nodes;
	std::vector<TrieNode> child_nodes;
	TrieNode root(*this, hash_, this->storage_, hash, false);
	size_t subtriesize = getSubTrie(root, maxNumberOfHashes, inner_nodes,
			child_nodes);
	unsigned char * pos = (unsigned char *) buffer;
	while (inner_nodes.size() > 0) {
		TrieNode back = inner_nodes.back();
		memcpy(pos, back.hash, this->hash_.getHashSize());
		pos += this->hash_.getHashSize();
		inner_nodes.pop_back();
	}
	while (child_nodes.size() > 0) {
		TrieNode back = child_nodes.back();
		memcpy(pos, back.hash, this->hash_.getHashSize());
		pos += this->hash_.getHashSize();
		child_nodes.pop_back();
	}
	return subtriesize * this->hash_.getHashSize();
}

bool KeyValueTrie::getRoot(unsigned char * hash) {
	if (this->getSize() == 0)
		return false;
	try {
		TrieNode root = this->root_->get();
		memcpy(hash, root.hash, this->hash_.getHashSize());
		return true;
	} catch (TrieException e) {
		return false;
	}
}

void KeyValueTrie::diff(const void * subtrie, const std::size_t length,
		setsync::AbstractDiffHandler& handler) const {
	unsigned char * subtrie_ = (unsigned char *) subtrie;
	for (std::size_t i = 0; i < length / hash_.getHashSize(); i++) {
		if (!contains(subtrie_ + i * hash_.getHashSize())) {
			handler(subtrie_ + i * hash_.getHashSize(), hash_.getHashSize(),
					false);
		}
	}
}

}
