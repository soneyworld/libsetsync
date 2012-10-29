/*
 * Set.cpp
 *
 *      Author: Till Lorentzen
 */

#include "Set.hpp"
#include "MemSet.h"
#include "setsync/bloom/CountingBloomFilter.h"
#include "setsync/trie/DBTrie.h"
#include <typeinfo>
#include <string.h>
#include <iostream>

namespace setsync {

Set::Set(const uint64_t maxSize, const bool hardMaximum,
		const std::size_t hashsize) :
	maxSize_(maxSize), hardMaximum_(hardMaximum), hashSize_(hashsize) {
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

bool Set::erase(const char * key) {
	unsigned char k[this->hashSize_];
	SHA1((const unsigned char*) key, strlen(key), k);
	return erase(k);
}

bool Set::erase(const std::string key) {
	return erase(key.c_str());
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

bool Set::insert(const char * key) {
	unsigned char k[this->hashSize_];
	SHA1((const unsigned char*) key, strlen(key), k);
	return insert(k);
}

bool Set::insert(const std::string key) {
	return insert(key.c_str());
}

bool Set::insert(const unsigned char * key) {
	if(hardMaximum_){
		if(this->trie_->getSize() >= maxSize_){
			return false;
		}
	}
	if (this->trie_->add(key)) {
		this->bf_->add(key);
		return true;
	}
	return false;
}

bool Set::find(const char * key) {
	unsigned char k[this->hashSize_];
	SHA1((const unsigned char*) key, strlen(key), k);
	return find(k);
}

bool Set::find(const std::string key) {
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

}

int set_init(SET *set) {
	setsync::Set * cppset = new setsync::MemSet();
	set->set = (void *) cppset;
}

int set_free(SET *set) {
	setsync::Set * cppset = static_cast<setsync::Set*> (set->set);
	delete cppset;
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

int set_erase(SET *set, const unsigned char * key) {
	setsync::Set * cppset = static_cast<setsync::Set*> (set->set);
	return cppset->erase(key);
}

int set_clear(SET *set) {
	setsync::Set * cppset = static_cast<setsync::Set*> (set->set);
	cppset->clear();
	return 0;

}
