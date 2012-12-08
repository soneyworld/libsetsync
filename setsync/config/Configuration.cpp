/*
 * Config.cpp
 *
 *      Author: Till Lorentzen
 */

#include "Configuration.h"

namespace setsync {

namespace config {

Configuration::Configuration(const BloomFilterConfig& bf,
		const TrieConfig& trie, const IndexConfig& index,
		const std::string hashname) :
	hashname(hashname), bfConfig_(bf), trieConfig_(trie), indexConfig_(index) {

}

Configuration::Configuration(const SET_CONFIG config) {

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

const std::string Configuration::getPath() const{
	return this->path_;
}
}

}
