/*
 * Set.cpp
 *
 *      Author: Till Lorentzen
 */

#include "Set.hpp"
#include <setsync/bloom/CountingBloomFilter.h>
#include <typeinfo>
#include <string.h>
#include <iostream>
#ifdef HAVE_LEVELDB
#include <setsync/storage/LevelDbStorage.h>
#endif
#ifdef HAVE_DB_CXX_H
#include <setsync/storage/BdbStorage.h>
#endif
#include <setsync/utils/bitset.h>

namespace setsync {

SynchronizationProcess::SynchronizationProcess(Set * set,
		AbstractDiffHandler * handler) :
	stat_(START), set_(set), handler_(handler), bloomfilterOut_pos(0),
			bloomfilterIn_pos(0), bfOutIsFinished_(false), sentBytes_(0),
			receivedBytes_(0) {
}

SynchronizationProcess::~SynchronizationProcess() {

}

std::size_t SynchronizationProcess::getSentBytes() const {
	return this->sentBytes_;
}
std::size_t SynchronizationProcess::getReceivedBytes() const {
	return this->receivedBytes_;
}

std::size_t SynchronizationProcess::calcOutputBufferSize(const size_t RTT,
		const size_t bandwidth) const {
	if (RTT == 0) {
		return this->set_->hash_.getHashSize() * 2;
	} else {
		double rtt = RTT;
		rtt = rtt / 1000000;
		double bw = bandwidth;
		double bits = bw * rtt;
		std::size_t result = (std::size_t) (bits / 8.0);
		return result;
	}
}

bool SynchronizationProcess::isBloomFilterOutputAvail() const {
	return !this->bfOutIsFinished_;
}

bool SynchronizationProcess::isAckOutputAvailable() const {
	return this->pendingAcks_.size() > 0;
}

std::size_t SynchronizationProcess::readSomeBloomFilter(unsigned char * buffer,
		const std::size_t length) {
	std::size_t written = this->set_->bf_->getChunk(buffer, length,
			this->bloomfilterOut_pos);
	this->bloomfilterOut_pos += written;
	if (written < length) {
		bfOutIsFinished_ = true;
	}
	this->sentBytes_ += written;
	return written;
}

void SynchronizationProcess::diffBloomFilter(const unsigned char * buffer,
		const std::size_t length, AbstractDiffHandler& handler) {
	this->set_->bf_->diff(buffer, length, bloomfilterIn_pos, handler);
	bloomfilterIn_pos += length;
	this->receivedBytes_ += length;
}

bool SynchronizationProcess::getRootHash(unsigned char * hash) {
	return this->set_->trie_->getRoot(hash);
}

bool SynchronizationProcess::getRootHashForSending(unsigned char * hash) {
	this->sentBytes_ += this->set_->getHashFunction().getHashSize();
	return getRootHash(hash);
}

bool SynchronizationProcess::isEqual(const unsigned char* hash) {
	if (this->stat_ == EQUAL) {
		return true;
	}
	unsigned char localroothash[this->set_->hash_.getHashSize()];
	if (!this->set_->trie_->getRoot(localroothash)) {
		return false;
	}
	if (memcmp(localroothash, hash, this->set_->hash_.getHashSize()) == 0) {
		this->stat_ = EQUAL;
		return true;
	} else {
		return false;
	}
}

size_t SynchronizationProcess::getSubTrie(unsigned char * buffer,
		const size_t buffersize) {
	if (this->pendingSubtries_.size() > 0) {
		std::size_t subtriesize;
		try {
			subtriesize = this->set_->trie_->getSubTrie(
					pendingSubtries_.front().get(), buffer, buffersize);
		} catch (...) {
			pendingSubtries_.pop();
			return getSubTrie(buffer, buffersize);
		}
		pendingSubtries_.pop();
		std::size_t hashsize = this->set_->getHashFunction().getHashSize();
		std::size_t entries = subtriesize / hashsize;
		for (std::size_t i = 0; i < entries; i++) {
			this->sentHashes_.push(
					utils::CryptoHashContainer(this->set_->getHashFunction(),
							buffer + i * hashsize));
		}
		this->sentBytes_ += subtriesize;
		return subtriesize;
	} else {
		return 0;
	}
}

size_t SynchronizationProcess::readSomeTrieAcks(unsigned char * buffer,
		const std::size_t length, std::size_t * numberOfAcks) {
	if (this->pendingAcks_.size() > 0) {
		memset(buffer, 0, length);
		std::size_t number = std::min(length * 8, this->pendingAcks_.size());
		*numberOfAcks = number;
		for (std::size_t i = 0; i < number; i++) {
			bool b = this->pendingAcks_.front();
			if (b)
				BITSET(buffer,i);
			this->pendingAcks_.pop();
		}
		std::size_t resultsize = (number + 7) / 8;
		this->sentBytes_ += resultsize;
		return resultsize;
	} else {
		return 0;
	}
}

size_t SynchronizationProcess::processSubTrie(const unsigned char * buffer,
		const std::size_t length) {
	std::size_t hashsize = this->set_->getHashFunction().getHashSize();
	std::size_t entries = length / hashsize;
	for (std::size_t i = 0; i < entries; i++) {
		if (this->set_->trie_->contains(buffer + i * hashsize)) {
			this->pendingAcks_.push(false);
		} else {
			this->pendingAcks_.push(true);
		}
	}
	this->receivedBytes_ += length;
	return (this->pendingAcks_.size() + 7) / 8;
}
void SynchronizationProcess::processAcks(const unsigned char * buffer,
		const std::size_t length, const std::size_t numberOfAcks,
		AbstractDiffHandler& handler) {
	if (length < (numberOfAcks + 7) / 8) {
		throw "illegal input length";
	}
	if (numberOfAcks > this->sentHashes_.size()) {
		throw "illegal input length";
	}
	for (std::size_t i = 0; i < numberOfAcks; i++) {
		if (BITTEST(buffer,i)) {
			trie::TrieNodeType type = this->set_->trie_->contains(
					this->sentHashes_.front().get());
			if (type == trie::LEAF_NODE) {
				handler.handle(this->sentHashes_.front().get(),
						this->set_->getHashFunction().getHashSize(), true);
			} else if (type == trie::INNER_NODE) {
				this->pendingSubtries_.push(this->sentHashes_.front());
			} else {
				// THIS SHOULD NEVER HAPPEN, BUT IF MULTIPLE INSTANCES ARE SYNCING,
				// THIS COULD HAPPEN AND IT RESTARTS THE TRIESYNC
				while (!this->pendingSubtries_.empty()) {
					this->pendingSubtries_.pop();
				}
				unsigned char
						newroot[this->set_->getHashFunction().getHashSize()];
				if (this->getRootHash(newroot))
					this->pendingSubtries_.push(
							utils::CryptoHashContainer(
									this->set_->getHashFunction(), newroot));

			}
		}
		this->sentHashes_.pop();
	}
	this->receivedBytes_ += (numberOfAcks + 7) / 8;
}

bool SynchronizationProcess::isSubtrieUnacked() const {
	return this->sentHashes_.size() > 0;
}

bool SynchronizationProcess::isSubtrieOutputAvailable() {
	if (this->stat_ == START) {
		unsigned char root[set_->getHashFunction().getHashSize()];
		if (this->getRootHash(root)) {
			this->stat_ = TRIE;
			this->pendingSubtries_.push(
					utils::CryptoHashContainer(this->set_->getHashFunction(),
							root));
			return isSubtrieOutputAvailable();
		} else {
			return false;
		}
	}
	return this->pendingSubtries_.size() > 0;
}

bool SynchronizationProcess::done() const {
	if (this->stat_ == EQUAL) {
		return true;
	}
	if (this->pendingAcks_.size() > 0) {
		return false;
	}
	if (this->pendingSubtries_.size() > 0) {
		return false;
	}
	if (this->sentHashes_.size() > 0) {
		return false;
	}
	if (this->stat_ == START) {
		return false;
	}
	return true;
}

std::size_t SynchronizationProcess::getMinTrieBuffer() const {
	return this->set_->getMinSyncTrieBuffer();
}

std::size_t SynchronizationProcess::getMinBFBuffer() const {
	return this->set_->getMinSyncBFBuffer();
}

std::size_t SynchronizationProcess::getMinBuffer() const {
	return this->set_->getMinSyncBuffer();
}

std::size_t Set::getMinSyncTrieBuffer() const {
	return trie_->getHash().getHashSize() * 2;
}

std::size_t Set::getMinSyncBFBuffer() const {
	return 1;
}

std::size_t Set::getMinSyncBuffer() const {
	return std::max(getMinSyncBFBuffer(), getMinSyncTrieBuffer());
}

std::string Set::getPath() const {
	if(this->tempDir!=NULL){
		return this->tempDir->getPath();
	}else{
		return config_.getPath();
	}
}

Set::Set(const config::Configuration& config) :
	hash_(config.getHashFunction()), config_(config), tempDir(NULL),
			indexInUse_(false) {
	if (config_.getPath().size() == 0) {
		tempDir = new utils::FileSystem::TemporaryDirectory("set_");
	}
#ifdef HAVE_DB_CXX_H
	this->bfdb = NULL;
	this->triedb = NULL;
	this->indexdb = NULL;
	this->env_ = NULL;
#endif
	switch (config_.getStorage().getType()) {
#ifdef HAVE_LEVELDB
	case config::Configuration::StorageConfig::LEVELDB: {
		std::string path;
		if (tempDir != NULL) {
			path = tempDir->getPath();
		} else {
			path = config_.getPath();
		}
		if (path.size() > 0 && path.at(path.size() - 1) != '/') {
			path = path.append("/");
		}
		std::string triepath(path);
		triepath.append("trie");
		trieStorage_ = new storage::LevelDbStorage(triepath);
		std::string bfpath(path);
		bfpath.append("bloom");
		bfStorage_ = new storage::LevelDbStorage(bfpath);
		std::string indexpath(path);
		indexpath.append("index");
		indexStorage_ = new storage::LevelDbStorage(indexpath);
	}
		break;
#endif
#ifdef HAVE_DB_CXX_H
	case config::Configuration::StorageConfig::BERKELEY_DB: {
		std::string path;
		if (tempDir != NULL) {
			path = tempDir->getPath();
		} else {
			path = config_.getPath();
		}
		this->env_ = new DbEnv(0);
		if (this->env_->open(path.c_str(), DB_INIT_MPOOL | DB_CREATE, 0) != 0) {
			this->env_->close(0);
			throw "Failed to open berkeley db database env";
		}
		this->bfdb = new Db(this->env_, 0);
		this->triedb = new Db(this->env_, 0);
		this->indexdb = new Db(this->env_, 0);
		this->bfdb->open(NULL, "set", "bf", DB_HASH, DB_CREATE, 0);
		this->triedb->open(NULL, "set", "trie", DB_HASH, DB_CREATE, 0);
		this->indexdb->open(NULL, "set", "in", DB_HASH, DB_CREATE, 0);
		trieStorage_ = new storage::BdbStorage(this->triedb);
		bfStorage_ = new storage::BdbStorage(this->bfdb);
		indexStorage_ = new storage::BdbStorage(this->indexdb);
	}
		break;
#endif
	default:
		throw "No storage type found!";
	}
	trie_ = new trie::KeyValueTrie(hash_, *trieStorage_);
	const config::Configuration::BloomFilterConfig& bfconfig =
			config_.getBloomFilter();
	std::string path;
	if (tempDir != NULL) {
		path = tempDir->getPath();
	} else {
		path = config_.getPath();
	}
	if (path.size() > 0 && path.at(path.size() - 1) != '/') {
		path = path.append("/");
	}
	std::string bffile(path);
	bffile.append("bloom.filter");
	bf_ = new bloom::KeyValueCountingBloomFilter(hash_, *bfStorage_, bffile,
			bfconfig.getMaxElements(), bfconfig.isHardMaximum(),
			bfconfig.falsePositiveRate);
	index_ = new setsync::index::KeyValueIndex(hash_, *indexStorage_);
	this->maxSize_ = bfconfig.getMaxElements();
	this->hardMaximum_ = bfconfig.isHardMaximum();
}

Set::~Set() {
	if (this->index_ != NULL) {
		delete index_;
	}
	if (this->bf_ != NULL) {
		delete bf_;
	}
	if (this->trie_ != NULL) {
		delete trie_;
	}
	if (this->indexStorage_ != NULL) {
		delete indexStorage_;
	}
	if (this->bfStorage_ != NULL) {
		delete bfStorage_;
	}
	if (this->trieStorage_ != NULL) {
		delete trieStorage_;
	}
#ifdef HAVE_DB_CXX_H
	if (this->triedb != NULL) {
		this->triedb->close(0);
		delete this->triedb;
	}
	if (this->bfdb != NULL) {
		this->bfdb->close(0);
		delete this->bfdb;
	}
	if (this->indexdb != NULL) {
		this->indexdb->close(0);
		delete this->indexdb;
	}
	if (this->env_ != NULL) {
		this->env_->close(0);
		delete this->env_;
	}
#endif
	if (this->tempDir != NULL) {
		delete this->tempDir;
	}
}

bool Set::isEmpty() const {
	return (getSize() == 0) ? true : false;
}

size_t Set::getSize() const {
	return trie_->getSize();
}
size_t Set::getMaximumSize() const {
	return this->maxSize_;
}

bool Set::erase(const char * str) {
	unsigned char k[this->hash_.getHashSize()];
	this->hash_(k, str);
	return erase(k);
}

bool Set::erase(const std::string& str) {
	return erase(str.c_str());
}

bool Set::erase(const unsigned char * key) {
	try {
		if (this->trie_->Trie::remove(key)) {
			bf_->remove(key);
			if (indexInUse_)
				index_->remove(key);
			return true;
		} else {
			return false;
		}
	} catch (const std::bad_cast& e) {
		return false;
	}
}

bool Set::erase(const void * data, const size_t length) {
	unsigned char k[this->hash_.getHashSize()];
	this->hash_(k, (const unsigned char*) data, length);
	return erase(k);
}

bool Set::insert(const char * str) {
	unsigned char k[this->hash_.getHashSize()];
	this->hash_(k, str);
	return insert(k);
}

bool Set::insert(const std::string& str) {
	return insert(str.c_str());
}

bool Set::insert(const unsigned char * key) {
	if (hardMaximum_) {
		if (this->trie_->getSize() >= maxSize_) {
			return false;
		}
	}
	if (this->trie_->Trie::add(key)) {
		this->bf_->add(key);
		return true;
	}
	return false;
}

bool Set::insert(const void * data, const std::size_t length) {
	unsigned char k[this->hash_.getHashSize()];
	this->hash_(k, (const unsigned char *) data, length);
	if (this->index_->put(k, data, length)) {
		bool success = insert(k);
		if (success) {
			this->indexInUse_ = true;
			return true;
		} else {
			this->index_->remove(k);
			return false;
		}
	} else {
		return false;
	}
}

bool Set::find(const char * str) {
	unsigned char k[this->hash_.getHashSize()];
	this->hash_(k, str);
	return find(k);
}

bool Set::find(const std::string& key) {
	return find(key.c_str());
}

bool Set::find(const unsigned char * key) {
	if (this->bf_->contains(key)) {
		return this->trie_->contains(key) == trie::LEAF_NODE;
	}
	return false;
}

bool Set::find(const void * data, const std::size_t length) {
	unsigned char k[this->hash_.getHashSize()];
	this->hash_(k, (const unsigned char*) data, length);
	return find(k);
}

bool Set::get(const unsigned char * key, unsigned char ** value,
		std::size_t * valueSize) {
	*value = NULL;
	*valueSize = 0;
	if (find(key)) {
		if (this->indexInUse_) {
			*valueSize = this->index_->getSizeOf(key);
			*value = (unsigned char*) malloc(*valueSize);
			this->index_->get(key, *value, valueSize);
		} else {
			return false;
		}
	} else {
		return false;
	}
	return false;
}

void Set::clear() {
	this->trie_->clear();
	this->bf_->clear();
	this->index_->clear();
	this->indexInUse_ = false;
}

const utils::CryptoHash& Set::getHashFunction() const {
	return this->hash_;
}

SynchronizationProcess * Set::createSyncProcess() {
	return new SynchronizationProcess(this);
}

bool Set::operator ==(const Set& other) const {
	return this->trie_->operator ==(*other.trie_);
}

} // end of namespace setsync

SET_CONFIG set_create_config() {
	SET_CONFIG c;
	c.bf_hard_max = false;
	c.bf_max_elements = 10000;
	c.false_positive_rate = 0.001;
	c.function = SHA_1;
#ifdef HAVE_LEVELDB
	c.storage = LEVELDB;
#else
	c.storage = BERKELEY_DB;
#endif
	return c;
}

int set_init(SET *set, SET_CONFIG config) {
	set->error = NULL;
	try {
		setsync::config::Configuration c(config);
		setsync::Set * cppset = new setsync::Set(c);
		set->set = (void *) cppset;
	} catch (std::exception& e) {
		if (set->error == NULL) {
			set->error = (void *) new std::string(e.what());
		} else {
			std::string * msg = static_cast<std::string *> (set->error);
			msg->operator =(e.what());
		}
		return -1;
	} catch (...) {
		if (set->error == NULL) {
			set->error = (void *) new std::string("unknown error");
		} else {
			std::string * msg = static_cast<std::string *> (set->error);
			msg->operator =("unknown error");
		}
		return -1;
	}
	return 0;
}

int set_init_with_path(SET *set, SET_CONFIG config, const char * path) {
	try {
		setsync::config::Configuration c(config);
		c.setPath(path);
		setsync::Set * cppset = new setsync::Set(c);
		set->set = (void *) cppset;
	} catch (std::exception& e) {
		if (set->error == NULL) {
			set->error = (void *) new std::string(e.what());
		} else {
			std::string * msg = static_cast<std::string *> (set->error);
			msg->operator =(e.what());
		}
		return -1;
	} catch (...) {
		if (set->error == NULL) {
			set->error = (void *) new std::string("unknown error");
		} else {
			std::string * msg = static_cast<std::string *> (set->error);
			msg->operator =("unknown error");
		}
		return -1;
	}
	return 0;
}

int set_free(SET *set) {
	try {
		setsync::Set * cppset = static_cast<setsync::Set*> (set->set);
		delete cppset;
		if (set->error != NULL) {
			std::string * msg = static_cast<std::string *> (set->error);
			delete msg;
		}
	} catch (std::exception& e) {
		if (set->error == NULL) {
			set->error = (void *) new std::string(e.what());
		} else {
			std::string * msg = static_cast<std::string *> (set->error);
			msg->operator =(e.what());
		}
		return -1;
	} catch (...) {
		std::cerr << "FATAL ERROR: FREE SET FAILED" << std::endl;
		return -1;
	}
	return 0;
}

// Capacity
int set_empty(SET *set) {
	setsync::Set * cppset = static_cast<setsync::Set*> (set->set);
	return cppset->isEmpty() ? 1 : 0;
}

size_t set_size(SET *set) {
	setsync::Set * cppset = static_cast<setsync::Set*> (set->set);
	return cppset->getSize();
}

size_t set_max_size(SET *set) {
	setsync::Set * cppset = static_cast<setsync::Set*> (set->set);
	return cppset->getMaximumSize();
}

// Lookup
int set_find(SET *set, const unsigned char * key) {
	setsync::Set * cppset = static_cast<setsync::Set*> (set->set);
	return cppset->find(key);
}

int set_find_string(SET *set, const char * str) {
	setsync::Set * cppset = static_cast<setsync::Set*> (set->set);
	return cppset->find(str);
}

int set_get_data(SET *set, const unsigned char * key, unsigned char ** value,
		size_t * valueSize) {
	setsync::Set * cppset = static_cast<setsync::Set*> (set->set);
	return cppset->get(key, value, valueSize);
}

// Modifiers
int set_insert(SET *set, const unsigned char * key) {
	setsync::Set * cppset = static_cast<setsync::Set*> (set->set);
	return cppset->insert(key);
}

int set_insert_string(SET *set, const char * str) {
	setsync::Set * cppset = static_cast<setsync::Set*> (set->set);
	return cppset->insert(str);
}

int set_insert_data(SET *set, const void* data, const size_t length) {
	setsync::Set * cppset = static_cast<setsync::Set*> (set->set);
	return cppset->insert(data, length);
}

int set_erase(SET *set, const unsigned char * key) {
	setsync::Set * cppset = static_cast<setsync::Set*> (set->set);
	return cppset->erase(key);
}

int set_erase_string(SET *set, const char * str) {
	setsync::Set * cppset = static_cast<setsync::Set*> (set->set);
	return cppset->erase(str);
}

int set_clear(SET *set) {
	try {
		setsync::Set * cppset = static_cast<setsync::Set*> (set->set);
		cppset->clear();
	} catch (std::exception& e) {
		if (set->error == NULL) {
			set->error = (void *) new std::string(e.what());
		} else {
			std::string * msg = static_cast<std::string *> (set->error);
			msg->operator =(e.what());
		}
		return -1;
	} catch (...) {
		if (set->error == NULL) {
			set->error = (void *) new std::string("unknown error");
		} else {
			std::string * msg = static_cast<std::string *> (set->error);
			msg->operator =("unknown error");
		}
		return -1;
	}
	return 0;
}

int set_sync_init_handle(SET * set, SET_SYNC_HANDLE * handle) {
	handle->error = NULL;
	try {
		setsync::Set * cppset = static_cast<setsync::Set*> (set->set);
		handle->process = (void*) cppset->createSyncProcess();
	} catch (std::exception& e) {
		if (set->error == NULL) {
			set->error = (void *) new std::string(e.what());
		} else {
			std::string * msg = static_cast<std::string *> (set->error);
			msg->operator =(e.what());
		}
		return -1;
	} catch (...) {
		if (set->error == NULL) {
			set->error = (void *) new std::string("unknown error");
		} else {
			std::string * msg = static_cast<std::string *> (set->error);
			msg->operator =("unknown error");
		}
		return -1;
	}
	return 0;
}

int set_sync_get_root_hash(SET_SYNC_HANDLE * handle, unsigned char * hash) {
	setsync::SynchronizationProcess * process =
			static_cast<setsync::SynchronizationProcess*> (handle->process);
	try {
		if (process->getRootHash(hash)) {
			return 0;
		} else {
			return -1;
		}
	} catch (std::exception& e) {
		if (handle->error == NULL) {
			handle->error = (void *) new std::string(e.what());
		} else {
			std::string * msg = static_cast<std::string *> (handle->error);
			msg->operator =(e.what());
		}
		return -1;
	} catch (...) {
		if (handle->error == NULL) {
			handle->error = (void *) new std::string("unknown error");
		} else {
			std::string * msg = static_cast<std::string *> (handle->error);
			msg->operator =("unknown error");
		}
		return -1;
	}
	return 0;
}

int set_sync_is_equal_to_hash(SET_SYNC_HANDLE * handle,
		const unsigned char * remotehash) {
	setsync::SynchronizationProcess * process =
			static_cast<setsync::SynchronizationProcess*> (handle->process);
	try {
		if (process->isEqual(remotehash)) {
			return 1;
		} else {
			return 0;
		}
	} catch (std::exception& e) {
		if (handle->error == NULL) {
			handle->error = (void *) new std::string(e.what());
		} else {
			std::string * msg = static_cast<std::string *> (handle->error);
			msg->operator =(e.what());
		}
		return -1;
	} catch (...) {
		if (handle->error == NULL) {
			handle->error = (void *) new std::string("unknown error");
		} else {
			std::string * msg = static_cast<std::string *> (handle->error);
			msg->operator =("unknown error");
		}
		return -1;
	}
}

int set_sync_bf_output_avail(SET_SYNC_HANDLE * handle) {
	setsync::SynchronizationProcess * process =
			static_cast<setsync::SynchronizationProcess*> (handle->process);
	try {
		if (process->isBloomFilterOutputAvail()) {
			return 1;
		}
	} catch (std::exception& e) {
		if (handle->error == NULL) {
			handle->error = (void *) new std::string(e.what());
		} else {
			std::string * msg = static_cast<std::string *> (handle->error);
			msg->operator =(e.what());
		}
		return -1;
	} catch (...) {
		if (handle->error == NULL) {
			handle->error = (void *) new std::string("unknown error");
		} else {
			std::string * msg = static_cast<std::string *> (handle->error);
			msg->operator =("unknown error");
		}
		return -1;
	}
	return 0;
}

size_t set_sync_bf_readsome(SET_SYNC_HANDLE * handle, unsigned char* buffer,
		const size_t buffersize) {
	setsync::SynchronizationProcess * process =
			static_cast<setsync::SynchronizationProcess*> (handle->process);
	try {
		return process->readSomeBloomFilter(buffer, buffersize);
	} catch (std::exception& e) {
		if (handle->error == NULL) {
			handle->error = (void *) new std::string(e.what());
		} else {
			std::string * msg = static_cast<std::string *> (handle->error);
			msg->operator =(e.what());
		}
		return 0;
	} catch (...) {
		if (handle->error == NULL) {
			handle->error = (void *) new std::string("unknown error");
		} else {
			std::string * msg = static_cast<std::string *> (handle->error);
			msg->operator =("unknown error");
		}
		return 0;
	}
	return 0;
}

int set_sync_bf_diff(SET_SYNC_HANDLE * handle, const unsigned char* inbuffer,
		const size_t inlength, diff_callback * callback, void * closure) {
	setsync::SynchronizationProcess * process =
			static_cast<setsync::SynchronizationProcess*> (handle->process);
	try {
		setsync::C_DiffHandler handler(callback, closure);
		process->diffBloomFilter(inbuffer, inlength, handler);
	} catch (std::exception& e) {
		if (handle->error == NULL) {
			handle->error = (void *) new std::string(e.what());
		} else {
			std::string * msg = static_cast<std::string *> (handle->error);
			msg->operator =(e.what());
		}
		return -1;
	} catch (...) {
		if (handle->error == NULL) {
			handle->error = (void *) new std::string("unknown error");
		} else {
			std::string * msg = static_cast<std::string *> (handle->error);
			msg->operator =("unknown error");
		}
		return -1;
	}
	return 0;
}

ssize_t set_sync_trie_get_subtrie(SET_SYNC_HANDLE * handle,
		unsigned char * buffer, const size_t length) {
	setsync::SynchronizationProcess * process =
			static_cast<setsync::SynchronizationProcess*> (handle->process);
	try {
		if (process->isSubtrieOutputAvailable()) {
			return process->getSubTrie(buffer, length);
		}
	} catch (std::exception& e) {
		if (handle->error == NULL) {
			handle->error = (void *) new std::string(e.what());
		} else {
			std::string * msg = static_cast<std::string *> (handle->error);
			msg->operator =(e.what());
		}
		return -1;
	} catch (...) {
		if (handle->error == NULL) {
			handle->error = (void *) new std::string("unknown error");
		} else {
			std::string * msg = static_cast<std::string *> (handle->error);
			msg->operator =("unknown error");
		}
		return -1;
	}
	return 0;
}

ssize_t set_sync_trie_process_subtrie(SET_SYNC_HANDLE * handle,
		unsigned char* buffer, const size_t length) {
	setsync::SynchronizationProcess * process =
			static_cast<setsync::SynchronizationProcess*> (handle->process);
	try {
		return process->processSubTrie(buffer, length);
	} catch (std::exception& e) {
		if (handle->error == NULL) {
			handle->error = (void *) new std::string(e.what());
		} else {
			std::string * msg = static_cast<std::string *> (handle->error);
			msg->operator =(e.what());
		}
		return -1;
	} catch (...) {
		if (handle->error == NULL) {
			handle->error = (void *) new std::string("unknown error");
		} else {
			std::string * msg = static_cast<std::string *> (handle->error);
			msg->operator =("unknown error");
		}
		return -1;
	}
	return 0;
}

int set_sync_trie_subtrie_output_avail(SET_SYNC_HANDLE * handle) {
	setsync::SynchronizationProcess * process =
			static_cast<setsync::SynchronizationProcess*> (handle->process);
	try {
		return process->isSubtrieOutputAvailable() ? 1 : 0;
	} catch (std::exception& e) {
		if (handle->error == NULL) {
			handle->error = (void *) new std::string(e.what());
		} else {
			std::string * msg = static_cast<std::string *> (handle->error);
			msg->operator =(e.what());
		}
		return -1;
	} catch (...) {
		if (handle->error == NULL) {
			handle->error = (void *) new std::string("unknown error");
		} else {
			std::string * msg = static_cast<std::string *> (handle->error);
			msg->operator =("unknown error");
		}
		return -1;
	}
	return 0;
}

int set_sync_trie_acks_avail(SET_SYNC_HANDLE * handle) {
	setsync::SynchronizationProcess * process =
			static_cast<setsync::SynchronizationProcess*> (handle->process);
	try {
		return process->isAckOutputAvailable() ? 1 : 0;
	} catch (std::exception& e) {
		if (handle->error == NULL) {
			handle->error = (void *) new std::string(e.what());
		} else {
			std::string * msg = static_cast<std::string *> (handle->error);
			msg->operator =(e.what());
		}
		return -1;
	} catch (...) {
		if (handle->error == NULL) {
			handle->error = (void *) new std::string("unknown error");
		} else {
			std::string * msg = static_cast<std::string *> (handle->error);
			msg->operator =("unknown error");
		}
		return -1;
	}
	return 0;
}

ssize_t set_sync_trie_read_acks(SET_SYNC_HANDLE * handle,
		unsigned char * buffer, const size_t length, size_t * numberOfAcks) {
	setsync::SynchronizationProcess * process =
			static_cast<setsync::SynchronizationProcess*> (handle->process);
	try {
		return process->readSomeTrieAcks(buffer, length, numberOfAcks);
	} catch (std::exception& e) {
		if (handle->error == NULL) {
			handle->error = (void *) new std::string(e.what());
		} else {
			std::string * msg = static_cast<std::string *> (handle->error);
			msg->operator =(e.what());
		}
		return -1;
	} catch (...) {
		if (handle->error == NULL) {
			handle->error = (void *) new std::string("unknown error");
		} else {
			std::string * msg = static_cast<std::string *> (handle->error);
			msg->operator =("unknown error");
		}
		return -1;
	}
	return 0;
}

int set_sync_trie_process_acks(SET_SYNC_HANDLE * handle,
		const unsigned char * buffer, const size_t length,
		const size_t numberOfAcks, diff_callback * callback, void * closure) {
	setsync::SynchronizationProcess * process =
			static_cast<setsync::SynchronizationProcess*> (handle->process);
	try {
		setsync::C_DiffHandler handler(callback, closure);
		process->processAcks(buffer, length, numberOfAcks, handler);
	} catch (std::exception& e) {
		if (handle->error == NULL) {
			handle->error = (void *) new std::string(e.what());
		} else {
			std::string * msg = static_cast<std::string *> (handle->error);
			msg->operator =(e.what());
		}
		return -1;
	} catch (...) {
		if (handle->error == NULL) {
			handle->error = (void *) new std::string("unknown error");
		} else {
			std::string * msg = static_cast<std::string *> (handle->error);
			msg->operator =("unknown error");
		}
		return -1;
	}
	return 0;
}

size_t set_sync_sent_bytes(SET_SYNC_HANDLE * handle) {
	setsync::SynchronizationProcess * process =
			static_cast<setsync::SynchronizationProcess*> (handle->process);
	return process->getSentBytes();
}

size_t set_sync_received_bytes(SET_SYNC_HANDLE * handle) {
	setsync::SynchronizationProcess * process =
			static_cast<setsync::SynchronizationProcess*> (handle->process);
	return process->getReceivedBytes();
}

int set_sync_done(SET_SYNC_HANDLE * handle) {
	setsync::SynchronizationProcess * process =
			static_cast<setsync::SynchronizationProcess*> (handle->process);
	return process->done();
}

int set_sync_free_handle(SET_SYNC_HANDLE * handle) {
	try {
		setsync::SynchronizationProcess * process =
				static_cast<setsync::SynchronizationProcess*> (handle->process);
		delete process;
		if (handle->error != NULL) {
			std::string * msg = static_cast<std::string *> (handle->error);
			delete msg;
		}
	} catch (std::exception& e) {
		if (handle->error == NULL) {
			handle->error = (void *) new std::string(e.what());
		} else {
			std::string * msg = static_cast<std::string *> (handle->error);
			msg->operator =(e.what());
		}
		return -1;
	} catch (...) {
		if (handle->error == NULL) {
			handle->error = (void *) new std::string("unknown error");
		} else {
			std::string * msg = static_cast<std::string *> (handle->error);
			msg->operator =("unknown error");
		}
		return -1;
	}
	return 0;
}

size_t set_sync_min_trie_buffer(SET_SYNC_HANDLE * handle) {
	setsync::SynchronizationProcess * process =
			static_cast<setsync::SynchronizationProcess*> (handle->process);
	return process->getMinTrieBuffer();
}
size_t set_sync_min_bf_buffer(SET_SYNC_HANDLE * handle) {
	setsync::SynchronizationProcess * process =
			static_cast<setsync::SynchronizationProcess*> (handle->process);
	return process->getMinBFBuffer();
}
size_t set_sync_min_buffer(SET_SYNC_HANDLE * handle) {
	setsync::SynchronizationProcess * process =
			static_cast<setsync::SynchronizationProcess*> (handle->process);
	return process->getMinBuffer();
}

size_t set_sync_calc_output_buffer_size(SET_SYNC_HANDLE * handle,
		const size_t RTT, const size_t bandwidth) {
	setsync::SynchronizationProcess * process =
			static_cast<setsync::SynchronizationProcess*> (handle->process);
	return process->calcOutputBufferSize(RTT, bandwidth);
}

const char * set_last_error_to_string(SET * set) {
	if (set->error != NULL) {
		std::string * msg = static_cast<std::string *> (set->error);
		return msg->c_str();
	} else {
		return NULL;
	}
}

int set_last_error_printf(SET * set) {
	if (set->error != NULL) {
		std::string * msg = static_cast<std::string *> (set->error);
		std::cout << (*msg);
		return 0;
	} else {
		return -1;
	}
}
