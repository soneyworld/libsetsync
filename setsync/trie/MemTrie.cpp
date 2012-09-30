/*
 * MemTrie.cpp
 *
 *      Author: Till Lorentzen
 */

#include "MemTrie.h"
#include <string.h>
#include <stdlib.h>

//=============================================================
//http://c-faq.com/misc/bitsets.html
#define BITS_PER_CHAR 8

#define BITMASK(b) (1 << ((b) % BITS_PER_CHAR))
#define BITSLOT(b) ((b) / BITS_PER_CHAR)
#define BITSET(a, b) ((a)[BITSLOT(b)] |= BITMASK(b))
#define BITCLEAR(a, b) ((a)[BITSLOT(b)] &= ~BITMASK(b))
#define BITTEST(a, b) ((a)[BITSLOT(b)] & BITMASK(b))
#define BITNSLOTS(nb) ((nb + BITS_PER_CHAR - 1) / BITS_PER_CHAR)
//=============================================================


namespace trie {

MemTrieNode::MemTrieNode(MemTrieNode * parent, Trie * trie) :
	parent_(parent), trie_(trie) {
	this->larger_ = NULL;
	this->smaller_ = NULL;
	this->hash_ = (unsigned char*) malloc(trie_->getHashSize());
	this->prefix_ = (unsigned char*) malloc(trie_->getHashSize());
	this->prefix_mask_ = trie_->getHashSize() * BITS_PER_CHAR;
}
MemTrieNode::~MemTrieNode() {
	free(this->hash_);
	free(this->prefix_);
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

bool MemTrieNode::remove(const unsigned char* hash, bool performhash){
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
	if(this->root_== NULL){
		return false;
	}else{
		this->root_->remove(hash, performhash);
	}
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
