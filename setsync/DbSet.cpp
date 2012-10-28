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
		const float falsePositiveRate, const std::size_t hashsize) {
	this->bfdb = new Db(NULL, 0);
	this->triedb = new Db(NULL, 0);
	this->bfdb->set_flags(bloom::DBBloomFilter::getTableFlags());
	this->bfdb->open(NULL, NULL,
			bloom::DBBloomFilter::getLogicalDatabaseName(),
			bloom::DBBloomFilter::getTableType(), DB_CREATE, 0);
	this->triedb->set_flags(trie::DBTrie::getTableFlags());
	this->triedb->open(NULL, NULL, trie::DBTrie::getLogicalDatabaseName(),
			trie::DBTrie::getTableType(), DB_CREATE, 0);
	this->bf_ = new bloom::DBBloomFilter(this->bfdb, maxSize, hardMaximum,
			falsePositiveRate, hashsize);
	this->trie_ = new trie::DBTrie(this->triedb, hashsize);
}

DbSet::~DbSet() {
	bloom::DBBloomFilter& f_ = dynamic_cast<bloom::DBBloomFilter&> (*bf_);
	f_.~DBBloomFilter();
	trie::DBTrie& t_ = dynamic_cast<trie::DBTrie&> (*trie_);
	t_.~DBTrie();
	this->bfdb->close(0);
	this->triedb->close(0);
	delete this->bfdb;
	delete this->triedb;
}

}
