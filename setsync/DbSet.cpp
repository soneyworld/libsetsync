/*
 * DbSet.cpp
 *
 *      Author: Till Lorentzen
 */

#include "DbSet.h"
#include <setsync/bloom/DBBloomFilter.h>
#include <setsync/trie/DBTrie.h>

namespace setsync {

DbSet::DbSet(const uint64_t maxSize, const bool hardMaximum,
		const std::size_t hashsize) {
	this->bfdb = new Db(NULL, 0);
	this->triedb = new Db(NULL, 0);
	this->bf_ = new bloom::DBBloomFilter(this->bfdb, maxSize, hardMaximum, hashsize);
	this->trie_ = new trie::DBTrie(this->triedb, hashsize);
}

DbSet::~DbSet() {
	delete this->trie_;
	delete this->bf_;
	this->bfdb->close(0);
	this->triedb->close(0);
	delete this->bfdb;
	delete this->triedb;
}

}
