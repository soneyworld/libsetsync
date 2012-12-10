/*
 * Set.cpp
 *
 *      Author: Till Lorentzen
 */

#include "Set.hpp"
#include <setsync/bloom/CountingBloomFilter.h>
#include <typeinfo>
#include <string.h>
#include <iostream>
#ifdef HAVE_LEVELDB
#include <setsync/storage/LevelDbStorage.h>
#endif
#ifdef HAVE_DB_CXX_H
#include <setsync/storage/BdbStorage.h>
#endif

namespace setsync {

Set::Set(const config::Configuration& config) :
	config_(config), hash_(config.getHashFunction()), tempDir(NULL),
			indexInUse_(false) {
	if (config_.getPath().size() == 0) {
		tempDir = new utils::FileSystem::TemporaryDirectory("set_");
	}
#ifdef HAVE_DB_CXX_H
	this->bfdb = NULL;
	this->triedb = NULL;
	this->indexdb = NULL;
	this->env_ = NULL;
#endif
	switch (config_.getStorage().getType()) {
#ifdef HAVE_LEVELDB
	case config::Configuration::StorageConfig::LEVELDB: {
		std::string path;
		if (tempDir != NULL) {
			path = tempDir->getPath();
		} else {
			path = config_.getPath();
		}
		if (path.size() > 0 && path.at(path.size() - 1) != '/') {
			path = path.append("/");
		}
		std::string triepath(path);
		triepath.append("trie");
		trieStorage_ = new storage::LevelDbStorage(triepath);
		std::string bfpath(path);
		bfpath.append("bloom");
		bfStorage_ = new storage::LevelDbStorage(bfpath);
		std::string indexpath(path);
		indexpath.append("index");
		indexStorage_ = new storage::LevelDbStorage(indexpath);
	}
		break;
#endif
#ifdef HAVE_DB_CXX_H
	case config::Configuration::StorageConfig::BERKELEY_DB: {
		std::string path;
		if (tempDir != NULL) {
			path = tempDir->getPath();
		} else {
			path = config_.getPath();
		}
		this->env_ = new DbEnv(0);
		if (this->env_->open(path.c_str(), DB_INIT_MPOOL | DB_CREATE, 0) != 0) {
			this->env_->close(0);
			throw "Failed to open berkeley db database env";
		}
		this->bfdb = new Db(this->env_, 0);
		this->triedb = new Db(this->env_, 0);
		this->indexdb = new Db(this->env_, 0);
		this->bfdb->open(NULL, "set", "bf", DB_HASH, DB_CREATE, 0);
		this->triedb->open(NULL, "set", "trie", DB_HASH, DB_CREATE, 0);
		this->indexdb->open(NULL, "set", "in", DB_HASH, DB_CREATE, 0);
		trieStorage_ = new storage::BdbStorage(this->triedb);
		bfStorage_ = new storage::BdbStorage(this->bfdb);
		indexStorage_ = new storage::BdbStorage(this->indexdb);
	}
		break;
#endif
	default:
		throw "No storage type found!";
	}
	trie_ = new trie::KeyValueTrie(hash_, *trieStorage_);
	const config::Configuration::BloomFilterConfig& bfconfig =
			config_.getBloomFilter();
	std::string path;
	if (tempDir != NULL) {
		path = tempDir->getPath();
	} else {
		path = config_.getPath();
	}
	if (path.size() > 0 && path.at(path.size() - 1) != '/') {
		path = path.append("/");
	}
	std::string bffile(path);
	bffile.append("bloom.filter");
	bf_ = new bloom::KeyValueCountingBloomFilter(hash_, *bfStorage_, bffile,
			bfconfig.getMaxElements(), bfconfig.isHardMaximum(),
			bfconfig.falsePositiveRate);
	index_ = new setsync::index::KeyValueIndex(hash_, *indexStorage_);
	this->maxSize_ = bfconfig.getMaxElements();
	this->hardMaximum_ = bfconfig.isHardMaximum();
}

Set::~Set() {
	if (this->index_ != NULL) {
		delete index_;
	}
	if (this->bf_ != NULL) {
		delete bf_;
	}
	if (this->trie_ != NULL) {
		delete trie_;
	}
	if (this->indexStorage_ != NULL) {
		delete indexStorage_;
	}
	if (this->bfStorage_ != NULL) {
		delete bfStorage_;
	}
	if (this->trieStorage_ != NULL) {
		delete trieStorage_;
	}
#ifdef HAVE_DB_CXX_H
	if (this->triedb != NULL) {
		this->triedb->close(0);
		delete this->triedb;
	}
	if (this->bfdb != NULL) {
		this->bfdb->close(0);
		delete this->bfdb;
	}
	if (this->indexdb != NULL) {
		this->indexdb->close(0);
		delete this->indexdb;
	}
	if (this->env_ != NULL) {
		this->env_->close(0);
		delete this->env_;
	}
#endif
	if (this->tempDir != NULL) {
		delete this->tempDir;
	}
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
		if (this->trie_->Trie::remove(key)) {
			bf_->remove(key);
			if (indexInUse_)
				index_->remove(key);
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
	unsigned char k[this->hash_.getHashSize()];
	this->hash_(k, (const unsigned char *) data, length);
	if (this->index_->put(k, data, length)) {
		bool success = insert(k);
		if (success) {
			this->indexInUse_ = true;
			return true;
		} else {
			this->index_->remove(k);
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
	this->index_->clear();
	this->indexInUse_ = false;
}

} // end of namespace setsync

SET_CONFIG set_create_config() {
	SET_CONFIG c;
	c.bf_hard_max = false;
	c.bf_max_elements = 10000;
	c.false_positive_rate = 0.001;
	c.function = SHA_1;
#ifdef HAVE_LEVELDB
	c.storage = LEVELDB;
#else
	c.storage = BERKELEY_DB;
#endif
	return c;
}

int set_init(SET *set, SET_CONFIG config) {
	try {
		setsync::config::Configuration c(config);
		setsync::Set * cppset = new setsync::Set(c);
		set->set = (void *) cppset;
	} catch (...) {
		return -1;
	}
	return 0;
}

int set_init_with_path(SET *set, SET_CONFIG config, const char * path) {
	try {
		setsync::config::Configuration c(config);
		c.setPath(path);
		setsync::Set * cppset = new setsync::Set(c);
		set->set = (void *) cppset;
	} catch (...) {
		return -1;
	}
	return 0;
}

int set_free(SET *set) {
	try {
		setsync::Set * cppset = static_cast<setsync::Set*> (set->set);
		delete cppset;
	} catch (...) {
		return -1;
	}
	return 0;
}

// Lookup
int set_find(SET *set, const unsigned char * key) {
	setsync::Set * cppset = static_cast<setsync::Set*> (set->set);
	return cppset->find(key);
}

int set_find_string(SET *set, const char * str) {
	setsync::Set * cppset = static_cast<setsync::Set*> (set->set);
	return cppset->find(str);
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
	try {
		setsync::Set * cppset = static_cast<setsync::Set*> (set->set);
		cppset->clear();
	} catch (...) {
		return -1;
	}
	return 0;
}
