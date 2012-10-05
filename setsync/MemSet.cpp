/*
 * MemSet.cpp
 *
 *      Author: Till Lorentzen
 */
#include "MemSet.h"
#include "setsync/trie/MemTrie.h"

namespace setsync {

MemSet::MemSet(const uint64_t maxSize, const bool hardMaximum,
		const std::size_t hashsize) :
	setsync::Set(maxSize, hardMaximum, hashsize) {
	this->bf_ = new bloom::BloomFilter(maxSize, hardMaximum);
	this->trie_ = new trie::MemTrie;
}

MemSet::~MemSet() {
	delete this->bf_;
	delete this->trie_;
}

}
