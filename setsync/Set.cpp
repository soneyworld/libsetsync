/*
 * Set.cpp
 *
 *      Author: Till Lorentzen
 */

#include "Set.hpp"
#include <setsync/bloom/CountingBloomFilter.h>
#include <setsync/trie/BdbTrie.h>
#include <typeinfo>
#include <string.h>
#include <iostream>
#ifdef HAVE_LEVELDB
#include <setsync/storage/LevelDbStorage.h>
#endif

namespace setsync {

Set::Set(const config::Configuration& config) :
	config_(config), hash_(config.getHashFunction()), indexStorage_(NULL),
			index_(NULL) {
#ifdef HAVE_LEVELDB
	std::string path = config_.getPath();
	if (path.size() > 0 && path.at(path.size() - 1) != '/') {
		path = path.append("/");
	}
	std::string triepath(path);
	triepath.append("trie");
	trieStorage_ = new storage::LevelDbStorage(triepath);
	std::string bfpath(path);
	bfpath.append("bloom");
	bfStorage_ = new storage::LevelDbStorage(bfpath);
#endif
	trie_ = new trie::KeyValueTrie(hash_, *trieStorage_);
	const config::Configuration::BloomFilterConfig& bfconfig =
			config_.getBloomFilter();
	bf_ = new bloom::KeyValueCountingBloomFilter(hash_, *bfStorage_,
			bfconfig.filterFile, bfconfig.getMaxElements(),
			bfconfig.hardMaximum, bfconfig.falsePositiveRate);
	this->maxSize_ = bfconfig.getMaxElements();
	this->hardMaximum_ = bfconfig.hardMaximum;
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
		if (this->trie_->Trie::remove(key)) {
			filter_.remove(key);
			return true;
		} else {
			return false;
		}
	} catch (const std::bad_cast& e) {
		return false;
	}
}

bool Set::erase(const void * data, const size_t length) {
	unsigned char k[this->hash_.getHashSize()];
	this->hash_(k, (const unsigned char*) data, length);
	return erase(k);
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
	if (this->trie_->Trie::add(key)) {
		this->bf_->add(key);
		return true;
	}
	return false;
}

bool Set::insert(const void * data, const std::size_t length) {
	if (config_.getIndex().enabled()) {
		unsigned char k[this->hash_.getHashSize()];
		this->hash_(k, (const unsigned char *) data, length);
		if (this->index_ == NULL) {
			if (this->indexStorage_ == NULL) {
			}
			this->index_ = new index::KeyValueIndex(hash_, *indexStorage_);
		}
		if (this->index_->put(k, data, length)) {
			bool success = insert(k);
			if (success) {
				return success;
			} else {
				this->index_->remove(k);
			}
			return false;
		} else {
			return false;
		}
	} else {
		return false;
	}
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

bool Set::find(const void * data, const std::size_t length) {
	unsigned char k[this->hash_.getHashSize()];
	this->hash_(k, (const unsigned char*) data, length);
	return find(k);
}

void Set::clear() {
	this->trie_->clear();
	this->bf_->clear();
	if (this->index_ != NULL) {
		this->index_->clear();
		delete this->index_;
		this->index_ = NULL;
		if (this->indexStorage_ != NULL) {
			delete this->indexStorage_;
			this->indexStorage_ = NULL;
		}
	}
}

SynchronizationStrategy Set::createSyncStrategy() {

}

}

int set_free(SET *set) {
	setsync::Set * cppset = static_cast<setsync::Set*> (set->set);
	cppset->~Set();
}

int set_init(SET *set, SET_CONFIG config) {
	setsync::config::Configuration c(config);
	setsync::Set * cppset = new setsync::Set(c);
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

int set_insert_data(SET *set, const void* data, const size_t length) {
	setsync::Set * cppset = static_cast<setsync::Set*> (set->set);
	return cppset->insert(data, length);
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
