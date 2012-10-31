/*
 * MemSet.cpp
 *
 *      Author: Till Lorentzen
 */
#include "MemSet.h"
#include "setsync/trie/MemTrie.h"

namespace setsync {

MemSet::MemSet(const utils::CryptoHash& hash, const uint64_t maxSize, const bool hardMaximum) :
	setsync::Set(hash, maxSize, hardMaximum) {
	this->bf_ = new bloom::BloomFilter(hash, maxSize, hardMaximum);
	this->trie_ = new trie::MemTrie(hash);
}

MemSet::~MemSet() {
	bloom::BloomFilter& filter_ = dynamic_cast<bloom::BloomFilter&> (*bf_);
	delete &filter_;
	trie::MemTrie& memtrie_ = dynamic_cast<trie::MemTrie&> (*trie_);
	delete &memtrie_;
}

}
