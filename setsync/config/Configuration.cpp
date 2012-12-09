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
			trieConfig_(TrieConfig()), indexConfig_(IndexConfig()) {

}

Configuration::Configuration(const IndexConfig& index,
		const std::string hashname) :
	hashname_(hashname), bfConfig_(BloomFilterConfig()),
			trieConfig_(TrieConfig()), indexConfig_(index) {

}
Configuration::Configuration(const BloomFilterConfig& bf,
		const std::string hashname) :
	hashname_(hashname), bfConfig_(bf), trieConfig_(TrieConfig()),
			indexConfig_(IndexConfig()) {

}
Configuration::Configuration(const TrieConfig& trie, const std::string hashname) :
	hashname_(hashname), bfConfig_(BloomFilterConfig()), trieConfig_(trie),
			indexConfig_(IndexConfig()) {

}
Configuration::Configuration(const BloomFilterConfig& bf,
		const TrieConfig& trie, const IndexConfig& index,
		const std::string hashname) :
	hashname_(hashname), bfConfig_(bf), trieConfig_(trie), indexConfig_(index) {

}

Configuration::Configuration(const SET_CONFIG config) {
	bfConfig_.falsePositiveRate = config.false_positive_rate;
	bfConfig_.hardMaximum_ = config.bf_hard_max;
	bfConfig_.maxElements_ = config.bf_max_elements;
	throw "not yet implemented";
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

Configuration::IndexConfig::IndexConfig() :
	enabled_(true) {

}

Configuration::IndexConfig::~IndexConfig() {

}

bool Configuration::IndexConfig::enabled() const {
	return this->enabled_;
}

const Configuration::IndexConfig& Configuration::getIndex() const {
	return this->indexConfig_;
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
