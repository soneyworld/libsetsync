/*
 * AbstractSetIndex.cpp
 *
 *      Author: Till Lorentzen
 */

#include "AbstractSetIndex.h"

namespace setsync {

namespace index {

AbstractSetIndex::AbstractSetIndex(const utils::CryptoHash& hash) :
	hash_(hash) {
}

AbstractSetIndex::~AbstractSetIndex() {
}

bool AbstractSetIndex::put(const void * data, const std::size_t length) {
	unsigned char md[this->hash_.getHashSize()];
	hash_(md, (const unsigned char*) data, length);
	return put(md, data, length);
}

}

}
