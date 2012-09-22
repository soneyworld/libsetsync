/*
 * MemTrie.cpp
 *
 *      Author: Till Lorentzen
 */

#include "MemTrie.h"
#include <string.h>

#define CHAR_BIT 8

#define BITMASK(b) (1 << ((b) % CHAR_BIT))
#define BITSLOT(b) ((b) / CHAR_BIT)
#define BITSET(a, b) ((a)[BITSLOT(b)] |= BITMASK(b))
#define BITCLEAR(a, b) ((a)[BITSLOT(b)] &= ~BITMASK(b))
#define BITTEST(a, b) ((a)[BITSLOT(b)] & BITMASK(b))
#define BITNSLOTS(nb) ((nb + CHAR_BIT - 1) / CHAR_BIT)

namespace trie {

MemTrieNode::MemTrieNode(MemTrieNode * parent, Trie * trie) :
	parent_(parent), trie_(trie) {
	this->larger_ = NULL;
	this->smaller_ = NULL;
	this->hash_ = new unsigned char[trie_->getHashSize()];
	this->prefix_ = new unsigned char[trie_->getHashSize()];
	this->prefix_mask_ = trie_->getHashSize() * CHAR_BIT;
}
MemTrieNode::~MemTrieNode() {
	delete this->hash_;
	delete this->prefix_;
}

uint8_t MemTrieNode::commonPrefixSize(MemTrieNode * node) {
	uint8_t common = 0;
	for (uint8_t i = 0; i < this->trie_->getHashSize(); i++) {
		if (BITTEST(this->prefix_, i) != BITTEST(node->prefix_, i))
			break;
		common++;
	}
	return common;
}

bool MemTrieNode::add(const unsigned char* hash, bool performhash) {
	return false;
}

void MemTrieNode::updateHash() {
	if (this->larger_ == NULL && this->smaller_ == NULL) {
		if (this->parent_ != NULL)
			this->parent_->updateHash();
		return;
	} else {
		memcpy(trie_->hashscratch, this->smaller_->hash_, trie_->getHashSize());
		memcpy(trie_->hashscratch + trie_->getHashSize(), this->larger_->hash_,
				trie_->getHashSize());
		SHA1(trie_->hashscratch, 2 * trie_->getHashSize(), this->hash_);
		if (this->parent_ != NULL)
			this->parent_->updateHash();

		return;
	}
}

MemTrie::MemTrie(const size_t hashsize) {
	this->setHashSize(hashsize);
	this->setSize(0);
	this->root_ = NULL;
}

MemTrie::~MemTrie() {
	this->clear();
}
bool MemTrie::add(const unsigned char * hash) {
	return add(hash, true);
}
bool MemTrie::add(const unsigned char * hash, bool performhash) {
	if (this->root_ == NULL) {
		this->root_ = new MemTrieNode(NULL, this);
		memcpy(this->root_->hash_, hash, this->getHashSize());
		memcpy(this->root_->prefix_, hash, this->getHashSize());
		incSize();
		return true;
	} else {
		if (this->root_->add(hash, performhash)) {
			incSize();
			return true;
		} else {
			return false;
		}
	}
}
bool MemTrie::remove(const unsigned char * hash) {
	return remove(hash, true);
}
bool MemTrie::remove(const unsigned char * hash, bool performhash) {
	decSize();
	return false;
}
void MemTrie::clear(MemTrieNode * node) {
	if (node->larger_ != NULL) {
		clear(node->larger_);
		clear(node->smaller_);
	}
	delete node;
}

void MemTrie::clear(void) {
	if (this->root_ != NULL) {
		this->clear(this->root_);
		delete this->root_;
		this->root_ = NULL;
	}
	this->setSize(0);
}

void MemTrie::performHash() {

}

}
