/*
 * DbSet.cpp
 *
 *      Author: Till Lorentzen
 */
#include <stdlib.h>
#include "DbSet.h"
#include <setsync/bloom/DBBloomFilter.h>
#include <setsync/trie/DBTrie.h>
#include <setsync/utils/FileSystem.h>

namespace setsync {

DbSet::DbSet(const utils::CryptoHash& hash, const char * db_home,
		const uint64_t maxSize, const bool hardMaximum,
		const float falsePositiveRate) :
	setsync::Set(hash, maxSize, hardMaximum) {
	this->env_ = new DbEnv(0);
	if (db_home == NULL) {
		this->db_temp_home = (char*) malloc(strlen("set_XXXXXX") + 1);
		strcpy(this->db_temp_home, "set_XXXXXX");
		this->db_temp_home = mkdtemp(this->db_temp_home);
		if (this->env_->open(this->db_temp_home, DB_INIT_MPOOL | DB_CREATE, 0)
				!= 0) {
			this->env_->close(0);
			throw "";
		}
	} else {
		this->db_temp_home = NULL;
		if (this->env_->open(db_home, DB_INIT_MPOOL | DB_CREATE, 0) != 0) {
			this->env_->close(0);
			throw "";
		}
	}

	this->bfdb = new Db(this->env_, 0);
	this->triedb = new Db(this->env_, 0);
	this->bfdb->set_flags(bloom::DBBloomFilter::getTableFlags());
	this->bfdb->open(NULL, "set",
			bloom::DBBloomFilter::getLogicalDatabaseName(),
			bloom::DBBloomFilter::getTableType(), DB_CREATE, 0);
	this->triedb->set_flags(trie::DBTrie::getTableFlags());
	this->triedb->open(NULL, "set", trie::DBTrie::getLogicalDatabaseName(),
			trie::DBTrie::getTableType(), DB_CREATE, 0);
	this->bf_ = new bloom::DBBloomFilter(this->hash_, this->bfdb, maxSize,
			hardMaximum, falsePositiveRate);
	this->trie_ = new trie::DBTrie(this->hash_, this->triedb);
}

DbSet::~DbSet() {
	bloom::DBBloomFilter& f_ = dynamic_cast<bloom::DBBloomFilter&> (*bf_);
	f_.~DBBloomFilter();
	trie::DBTrie& t_ = dynamic_cast<trie::DBTrie&> (*trie_);
	t_.~DBTrie();
	this->bfdb->close(0);
	this->triedb->close(0);
	this->env_->close(0);
	delete this->bfdb;
	delete this->triedb;
	delete this->env_;
	if (this->db_temp_home != NULL) {
		utils::FileSystem::rmDirRecursive(this->db_temp_home);
		delete this->db_temp_home;
	}

}

SynchronizationStrategy DbSet::createSyncStrategy(){

}

}
