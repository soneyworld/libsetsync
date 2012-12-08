/*
 * Config.h
 *
 *      Author: Till Lorentzen
 */

#ifndef CONFIG_H_
#define CONFIG_H_
#include "config.h"
#include <cstddef>
#include <stdint.h>
#include <string>
#include <setsync/utils/CryptoHash.h>
#include <setsync/set.h>
#ifdef HAVE_IBRCOMMON
//#include <ibrcommon/data/ConfigFile.h>
#endif

namespace setsync {

namespace config {

class Configuration {
public:
	class BloomFilterConfig {
	private:
		uint64_t maxElements;
	public:
		enum BloomFilterType {
			NORMAL = 0, COMPRESSED = 1
		};

		uint64_t getMaxElements() const;
		void setMaxElements(uint64_t max);
		bool hardMaximum;
		float falsePositiveRate;
		std::string storagePath;
		std::string filterFile;
	};
	class TrieConfig {
	public:
		TrieConfig();
		virtual ~TrieConfig();
	};
	class IndexConfig {
	private:
		bool enabled_;
	public:
		IndexConfig();
		virtual ~IndexConfig();
		virtual bool enabled() const;
	};
private:
	std::string path_;
	std::string dbname_;
	std::string hashname_;
public:
	const std::string getPath() const;
	void setPath(const char * path);
	void setPath(const std::string& path);
	Configuration(
			const std::string hashname = utils::CryptoHash::getDefaultName());
	Configuration(const IndexConfig& index,
			const std::string hashname = utils::CryptoHash::getDefaultName());
	Configuration(const BloomFilterConfig& bf,
			const std::string hashname = utils::CryptoHash::getDefaultName());
	Configuration(const TrieConfig& trie,
			const std::string hashname = utils::CryptoHash::getDefaultName());
	Configuration(const BloomFilterConfig& bf, const IndexConfig& index,
			const std::string hashname = utils::CryptoHash::getDefaultName());
	Configuration(const TrieConfig& trie, const IndexConfig& index,
			const std::string hashname = utils::CryptoHash::getDefaultName());
	Configuration(const BloomFilterConfig& bf, const TrieConfig& trie,
			const IndexConfig& index,
			const std::string hashname = utils::CryptoHash::getDefaultName());
	Configuration(const SET_CONFIG config);
	virtual ~Configuration();
	std::string getHashFunction() const;
	const Configuration::IndexConfig& getIndex() const;
	const Configuration::BloomFilterConfig& getBloomFilter() const;
	const Configuration::TrieConfig& getTrie() const;
private:
	IndexConfig indexConfig_;
	BloomFilterConfig bfConfig_;
	TrieConfig trieConfig_;
};

}

}

#endif /* CONFIG_H_ */
