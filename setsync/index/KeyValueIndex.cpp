/*
 * KeyValueIndex.cpp
 *
 *      Author: Till Lorentzen
 */

#include "KeyValueIndex.h"
#include <string.h>

namespace setsync {

namespace index {

KeyValueIndex::KeyValueIndex(const utils::CryptoHash& hash,
		setsync::storage::AbstractKeyValueStorage& storage) :
	AbstractSetIndex(hash), storage_(storage) {

}

KeyValueIndex::~KeyValueIndex() {
}

void KeyValueIndex::clear() {
	this->storage_.clear();
}

bool KeyValueIndex::remove(const unsigned char* hash) {
	try {
		this->storage_.del(hash, this->AbstractSetIndex::hash_.getHashSize());
		return true;
	} catch (...) {
		return false;
	}
}

size_t KeyValueIndex::getSizeOf(const unsigned char * hash) const {
	unsigned char * resultBuffer;
	std::size_t resultSize;
	if (this->storage_.get(hash, this->AbstractSetIndex::hash_.getHashSize(),
			&resultBuffer, &resultSize)) {
		free(resultBuffer);
	}
	return resultSize;
}

bool KeyValueIndex::get(const unsigned char * hash, void * buffer,
		size_t * buffersize) const {
	unsigned char * resultBuffer;
	std::size_t resultSize;
	if (this->storage_.get(hash, this->AbstractSetIndex::hash_.getHashSize(),
			&resultBuffer, &resultSize)) {
		memcpy(buffer, resultBuffer, std::min(resultSize, *buffersize));
		free(resultBuffer);
		*buffersize = std::min(resultSize, *buffersize);
		return true;
	} else {
		return false;
	}
}

bool KeyValueIndex::put(const unsigned char * hash, const void * data,
		const std::size_t length) {
	try {
		this->storage_.put(hash, this->AbstractSetIndex::hash_.getHashSize(),
				(const unsigned char*) data, length);
	} catch (...) {
		return false;
	}
	return true;
}
}

}
