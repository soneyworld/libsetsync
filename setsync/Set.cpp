/*
 * Set.cpp
 *
 *      Author: Till Lorentzen
 */

#include "Set.hpp"
#include <setsync/DbSet.h>
#include <setsync/bloom/CountingBloomFilter.h>
#include <setsync/trie/DBTrie.h>
#include <typeinfo>
#include <string.h>
#include <iostream>

namespace setsync {

Set::Set(const utils::CryptoHash& hash, const uint64_t maxSize,
		const bool hardMaximum) :
	hash_(hash), maxSize_(maxSize), hardMaximum_(hardMaximum) {

}

Set::~Set() {
}

bool Set::isEmpty() const {
	return (getSize() == 0) ? true : false;
}

size_t Set::getSize() const {
	return trie_->getSize();
}
size_t Set::getMaximumSize() const {
	return this->maxSize_;
}

bool Set::erase(const char * str) {
	unsigned char k[this->hash_.getHashSize()];
	this->hash_(k, str);
	return erase(k);
}

bool Set::erase(const std::string& str) {
	return erase(str.c_str());
}

bool Set::erase(const unsigned char * key) {
	try {
		bloom::CountingBloomFilter& filter_ =
				dynamic_cast<bloom::CountingBloomFilter&> (*bf_);
		if (this->trie_->remove(key)) {
			filter_.remove(key);
			return true;
		} else {
			return false;
		}
	} catch (const std::bad_cast& e) {
		return false;
	}
}

bool Set::insert(const char * str) {
	unsigned char k[this->hash_.getHashSize()];
	this->hash_(k, str);
	return insert(k);
}

bool Set::insert(const std::string& str) {
	return insert(str.c_str());
}

bool Set::insert(const unsigned char * key) {
	if (hardMaximum_) {
		if (this->trie_->getSize() >= maxSize_) {
			return false;
		}
	}
	if (this->trie_->add(key)) {
		this->bf_->add(key);
		return true;
	}
	return false;
}

bool Set::find(const char * str) {
	unsigned char k[this->hash_.getHashSize()];
	this->hash_(k, str);
	return find(k);
}

bool Set::find(const std::string& key) {
	return find(key.c_str());
}

bool Set::find(const unsigned char * key) {
	if (this->bf_->contains(key)) {
		return this->trie_->contains(key);
	}
	return false;
}

void Set::clear() {
	this->trie_->clear();
	this->bf_->clear();
}

SynchronizationStrategy Set::createSyncStrategy(){

}

}

int set_free(SET *set) {
	setsync::Set * cppset = static_cast<setsync::DbSet*> (set->set);
	cppset->~Set();
	utils::CryptoHash * cpphash = static_cast<utils::CryptoHash*> (set->hash);
	cpphash->~CryptoHash();
}

int set_init(SET *set, const char * path, const size_t maxSize,
		const int hardMaximum, const float falsePositiveRate) {
	utils::CryptoHash * hashfunction = new utils::CryptoHash();
	set->hash = (void *) hashfunction;
	bool max = hardMaximum ? true : false;
	setsync::Set * cppset = new setsync::DbSet(*hashfunction, path, maxSize,
			max, falsePositiveRate);
	set->set = (void *) cppset;

}

// Lookup
int find(SET *set, const unsigned char * key) {
	setsync::Set * cppset = static_cast<setsync::Set*> (set->set);
	return cppset->find(key);
}

// Modifiers
int set_insert(SET *set, const unsigned char * key) {
	setsync::Set * cppset = static_cast<setsync::Set*> (set->set);
	return cppset->insert(key);
}

int set_insert_string(SET *set, const char * str) {
	setsync::Set * cppset = static_cast<setsync::Set*> (set->set);
	return cppset->insert(str);
}

int set_erase(SET *set, const unsigned char * key) {
	setsync::Set * cppset = static_cast<setsync::Set*> (set->set);
	return cppset->erase(key);
}

int set_clear(SET *set) {
	setsync::Set * cppset = static_cast<setsync::Set*> (set->set);
	cppset->clear();
	return 0;

}
