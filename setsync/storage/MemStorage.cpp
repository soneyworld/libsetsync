/*
 * MemStorage.cpp
 *
 *      Author: Till Lorentzen
 */

#include "MemStorage.h"

namespace setsync {

namespace storage {

MemIterator::MemIterator(const std::map<MemEntry, MemEntry>& map) :
	map_(map), valid_(true), iter_(map_.begin()) {

}

MemIterator::~MemIterator() {

}

MemStorage::MemStorage() {
}

MemStorage::~MemStorage() {
}

void MemStorage::put(const unsigned char * key, const std::size_t keySize,
		const unsigned char * value, const std::size_t valueSize) {
	MemEntry k(key, keySize);
	MemEntry v(value, valueSize);
	this->map[k] = v;
}

bool MemStorage::get(const unsigned char * key, const std::size_t length,
		unsigned char ** value, std::size_t * valueSize) const {
	MemEntry k(key, length);
	this->map.find(k);
}

void MemStorage::del(const unsigned char * key, const std::size_t keySize) {
	MemEntry k(key, keySize);
	this->map.erase(k);
}

void MemStorage::clear(void) {
	this->map.clear();
}

AbstractKeyValueIterator * MemStorage::createIterator() {
	return new MemIterator(this->map);
}

}

}
