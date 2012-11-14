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

MemTrieNode::MemTrieNode(Trie * trie) :
	trie_(trie) {
	this->parent_ = NULL;
	this->larger_ = NULL;
	this->smaller_ = NULL;
	this->hash_ = new unsigned char[trie->getHash().getHashSize()];
	this->prefix_ = new unsigned char[trie->getHash().getHashSize()];
	this->prefix_mask_ = trie->getHash().getHashSize() * BITS_PER_CHAR;
}
MemTrieNode::~MemTrieNode() {
	delete this->hash_;
	delete this->prefix_;
}

uint8_t MemTrieNode::commonPrefixSize(MemTrieNode * node) {
	uint8_t common = 0;
	for (uint8_t i = 0; i < this->trie_->getHash().getHashSize(); i++) {
		if (BITTEST(this->prefix_, i) != BITTEST(node->prefix_, i))
			break;
		common++;
	}
	return common;
}

bool MemTrieNode::similar(unsigned char *a) {
	for (uint8_t i = 0; i < this->prefix_mask_; i++) {
		if (BITTEST(a, i) != BITTEST(this->prefix_,i)) {
			return false;
		}
	}
	return true;
}

bool MemTrieNode::remove(const unsigned char* hash, bool performhash) {
	return false;
}

void MemTrieNode::updateHash() {
	if (this->larger_ == NULL && this->smaller_ == NULL) {
		if (this->parent_ != NULL)
			this->parent_->updateHash();
		return;
	} else {
		const utils::CryptoHash& chash = trie_->getHash();
		memcpy(trie_->hashscratch, this->smaller_->hash_, chash.getHashSize());
		memcpy(trie_->hashscratch + chash.getHashSize(), this->larger_->hash_,
				chash.getHashSize());
		chash(this->hash_, trie_->hashscratch, 2 * chash.getHashSize());
		if (this->parent_ != NULL)
			this->parent_->updateHash();

		return;
	}
}

MemTrie::MemTrie(const utils::CryptoHash& hash) :
	Trie(hash) {
	this->setSize(0);
	this->root_ = NULL;
}

MemTrie::~MemTrie() {
	this->clear();
}

bool MemTrie::add(const unsigned char * hash, bool performhash) {
	MemTrieNode * newnode = new MemTrieNode(this);
	if (this->root_ == NULL) {
		this->root_ = newnode;
		memcpy(this->root_->hash_, hash, this->hash_.getHashSize());
		memcpy(this->root_->prefix_, hash, this->hash_.getHashSize());
		incSize();
		return true;
	} else {
		MemTrieNode * currentnode = this->root_;
		while (true) {
			bool thesame = currentnode->similar(newnode->prefix_);
			if (thesame) {
				MemTrieNode * next;
				bool
						larger =
								BITTEST( (unsigned char *)(newnode->prefix_) ,currentnode->prefix_mask_);
				if (larger)
					next = currentnode->larger_;
				else
					next = currentnode->smaller_;
				if (next == NULL) {
					if (currentnode->prefix_mask_ == this->hash_.getHashSize()
							* BITS_PER_CHAR) {
						//ignore
						return false;
					}
					throw "missing child!";
				}
				currentnode = next;
			} else {
				// Split
				uint8_t common = currentnode->commonPrefixSize(newnode);
				MemTrieNode * intermediate = new MemTrieNode(this);
				intermediate->parent_ = currentnode->parent_;
				bool rootReplaced = false;
				if (currentnode->parent_ == NULL) {
					this->root_ = intermediate;
					rootReplaced = true;
				}
				if (!rootReplaced) {
					if (currentnode->parent_->larger_ == currentnode) {
						currentnode->parent_->larger_ = intermediate;
					} else {
						currentnode->parent_->smaller_ = intermediate;
					}
				}

				intermediate->prefix_mask_ = common;
				memcpy(intermediate->prefix_, currentnode->prefix_,
						this->hash_.getHashSize());
				newnode->parent_ = intermediate;
				currentnode->parent_ = intermediate;
				bool
						larger =
								BITTEST( (unsigned char *)(newnode->prefix_) ,common);
				if (larger) {
					intermediate->larger_ = newnode;
					intermediate->smaller_ = currentnode;
				} else {
					intermediate->larger_ = currentnode;
					intermediate->smaller_ = newnode;
				}
				if (performhash)
					currentnode->updateHash();
				incSize();
				return true;
			}
		}
	}
}

bool MemTrie::remove(const unsigned char * hash, bool performhash) {
	if (this->root_ == NULL) {
		return false;
	} else {
		bool result = this->root_->remove(hash, performhash);
		if (result) {
			decSize();
		}
		return result;
	}
}

bool MemTrie::contains(const unsigned char * hash) const {
	return false;
}

void MemTrie::clear(MemTrieNode * node) {
	if (node->larger_ != NULL) {
		clear(node->larger_);
		clear(node->smaller_);
		delete node->larger_;
		node->larger_ = NULL;
		delete node->smaller_;
		node->smaller_ = NULL;
	}
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

bool MemTrie::operator ==(const Trie& other) const {
	//TODO
	return false;
}

size_t MemTrie::getSubTrie(const unsigned char * hash, void * buffer,
		const size_t buffersize) {
	//TODO
	return 0;
}
bool MemTrie::getRoot(unsigned char * hash){
	//TODO
	return false;
}

}
