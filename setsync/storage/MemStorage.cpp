/*
 * MemStorage.cpp
 *
 *      Author: Till Lorentzen
 */

#include "MemStorage.h"

namespace setsync {

namespace storage {

MemStorage::MemStorage(const std::size_t gbcache, const std::size_t cache) :
	inMemoryDb_(NULL), storage_(NULL) {
	this->inMemoryDb_ = new Db(NULL, 0);
	this->inMemoryDb_->set_cachesize(gbcache, cache, 0);
	this->inMemoryDb_->open(NULL, NULL, NULL, DB_BTREE, DB_CREATE, 0);
	this->storage_ = new BdbStorage(this->inMemoryDb_);
}

MemStorage::~MemStorage() {
	if (storage_ != NULL)
		delete storage_;
	if (inMemoryDb_ != NULL) {
		this->inMemoryDb_->close(0);
		delete this->inMemoryDb_;
	}
}

void MemStorage::put(const unsigned char * key, const std::size_t keySize,
		const unsigned char * value, const std::size_t valueSize) {
	this->storage_->put(key, keySize, value, valueSize);
}

bool MemStorage::get(const unsigned char * key, const std::size_t length,
		unsigned char ** value, std::size_t * valueSize) const {
	return this->storage_->get(key, length, value, valueSize);
}

void MemStorage::del(const unsigned char * key, const std::size_t keySize) {
	this->storage_->del(key, keySize);
}

void MemStorage::clear(void) {
	this->storage_->clear();
}

AbstractKeyValueIterator * MemStorage::createIterator() {
	return this->storage_->createIterator();
}

}

}
