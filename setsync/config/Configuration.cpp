/*
 * Config.cpp
 *
 *      Author: Till Lorentzen
 */

#include "Configuration.h"

namespace setsync {

namespace config {

Configuration::TrieConfig::TrieConfig() {

}

Configuration::TrieConfig::~TrieConfig() {

}

uint64_t Configuration::BloomFilterConfig::getMaxElements() const {
	return this->maxElements_;
}

void Configuration::BloomFilterConfig::setMaxElements(uint64_t max) {
	this->maxElements_ = max;
}

Configuration::Configuration(const std::string hashname) :
	hashname_(hashname), bfConfig_(BloomFilterConfig()),
			trieConfig_(TrieConfig()) {

}

Configuration::Configuration(const BloomFilterConfig& bf,
		const std::string hashname) :
	hashname_(hashname), bfConfig_(bf), trieConfig_(TrieConfig()) {

}
Configuration::Configuration(const TrieConfig& trie, const std::string hashname) :
	hashname_(hashname), bfConfig_(BloomFilterConfig()), trieConfig_(trie){

}
Configuration::Configuration(const BloomFilterConfig& bf,
		const TrieConfig& trie,
		const std::string hashname) :
	hashname_(hashname), bfConfig_(bf), trieConfig_(trie) {

}

Configuration::Configuration(const SET_CONFIG config) {
	bfConfig_.falsePositiveRate = config.false_positive_rate;
	bfConfig_.hardMaximum_ = config.bf_hard_max;
	bfConfig_.maxElements_ = config.bf_max_elements;
	switch (config.function) {
	case SHA_1:
		this->hashname_ = "sha1";
		break;
	case MD_2:
		this->hashname_ = "md2";
		break;
	case MD_4:
		this->hashname_ = "md4";
		break;
	case MD_5:
		this->hashname_ = "md5";
		break;
	case SHA_224:
		this->hashname_ = "sha224";
		break;
	case SHA_256:
		this->hashname_ = "sha256";
		break;
	case SHA_384:
		this->hashname_ = "sha384";
		break;
	case SHA_512:
		this->hashname_ = "sha512";
		break;
	}
	switch (config.storage) {
#ifdef HAVE_LEVELDB
	case LEVELDB:
		this->storageConfig_.type_ = Configuration::StorageConfig::LEVELDB;
		break;
#endif
#ifdef HAVE_DB_CXX_H
	case BERKELEY_DB:
		this->storageConfig_.type_ = Configuration::StorageConfig::BERKELEY_DB;
		break;
#endif
	default:
		break;
	}
}

std::string Configuration::getHashFunction() const {
	try {
		utils::CryptoHash hash(hashname_);
	} catch (...) {
		return utils::CryptoHash::getDefaultName();
	}
	return hashname_;
}

void Configuration::setPath(const char * path) {
	this->path_ = path;
}
void Configuration::setPath(const std::string& path) {
	setPath(path.c_str());
}

Configuration::~Configuration() {

}

const Configuration::BloomFilterConfig& Configuration::getBloomFilter() const {
	return this->bfConfig_;
}
const Configuration::TrieConfig& Configuration::getTrie() const {
	return this->trieConfig_;
}
const Configuration::StorageConfig& Configuration::getStorage() const {
	return this->storageConfig_;
}

const std::string Configuration::getPath() const {
	return this->path_;
}
}

}
