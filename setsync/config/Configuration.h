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
		friend class Configuration;
	public:
		enum BloomFilterType {
			NORMAL = 0, COMPRESSED = 1
		};

	private:
		uint64_t maxElements_;
		BloomFilterType type_;
		bool hardMaximum_;
	public:
		BloomFilterConfig(const uint64_t maxNumberOfElements = 10000,
				const bool hardMaximum = false,
				const float falsePositiveRate = 0.001) :
			maxElements_(maxNumberOfElements), hardMaximum_(hardMaximum),
					falsePositiveRate(falsePositiveRate) {
		}
		virtual ~BloomFilterConfig() {
		}
		uint64_t getMaxElements() const;
		void setMaxElements(uint64_t max);
		bool isHardMaximum(void) const{
			return this->hardMaximum_;
		}
		void setHardMaximum(bool isHardMax) {
			this->hardMaximum_ = isHardMax;
		}
		float falsePositiveRate;
	};
	class TrieConfig {
		friend class Configuration;
	public:
		TrieConfig();
		virtual ~TrieConfig();
	};
	class IndexConfig {
		friend class Configuration;
	private:
		bool enabled_;
	public:
		IndexConfig();
		virtual ~IndexConfig();
		virtual bool enabled() const;
	};
	class StorageConfig {
		friend class Configuration;
	public:
		enum StorageType {
			BERKELEY_DB = 0, LEVELDB = 1
		};
	private:
		StorageType type_;
	public:

#ifdef HAVE_LEVELDB
		StorageConfig(const StorageType type = LEVELDB) :
			type_(type) {
#else
#ifdef HAVE_DB_CXX_H
		StorageConfig(const StorageType type = BERKELEY_DB) :
			type_(type) {
#endif
#endif
		}
		virtual ~StorageConfig() {
		}
		StorageType getType(void) const {
			return this->type_;
		}
	};
private:
	std::string path_;
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
	const Configuration::StorageConfig& getStorage() const;
private:
	IndexConfig indexConfig_;
	BloomFilterConfig bfConfig_;
	TrieConfig trieConfig_;
	StorageConfig storageConfig_;
};

}

}

#endif /* CONFIG_H_ */
