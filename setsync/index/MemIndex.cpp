/*
 * MemIndex.cpp
 *
 *      Author: Till Lorentzen
 */

#include "MemIndex.h"
#include <string.h>

namespace setsync {

namespace index {

HashCompare::HashCompare(const std::size_t hashSize) :
	size_(hashSize) {
}

bool HashCompare::operator()(const unsigned char * key1,
		const unsigned char * key2) const {
	for (int i = 0; i < size_; i++) {
		if (key1[i] > key2[i]) {
			return true;
		} else if (key1[i] < key2[i]) {
			return false;
		}
	}
	return false;
}

MemIndex::MemIndex(const utils::CryptoHash& hash) :
	AbstractSetIndex(hash) {
	this->hashmap = new std::map<unsigned char *, void *, HashCompare>(
			HashCompare(hash_.getHashSize()));
}

MemIndex::~MemIndex() {
	delete this->hashmap;
}

bool MemIndex::put(const unsigned char * hash, const void * data,
		const std::size_t length) {
//	void * d = malloc(length);

	//TODO implement
	throw "not yet implemented";
}
void * MemIndex::get(const unsigned char * hash, std::size_t * length) const {
	//TODO implement
	throw "not yet implemented";

}
bool MemIndex::remove(const unsigned char * hash) {
	//TODO implement
	throw "not yet implemented";

}
void MemIndex::clear(void) {
	//TODO implement
	throw "not yet implemented";

}

}

}
