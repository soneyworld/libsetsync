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
#include <setsync/crypto/CryptoHash.h>
#include <setsync/set.h>
#ifdef HAVE_IBRCOMMON
#include <ibrcommon/data/ConfigFile.h>
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
		bool isHardMaximum(void) const {
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
	class StorageConfig {
		friend class Configuration;
	public:
		enum StorageType {
			BERKELEY_DB = 0, LEVELDB = 1, IN_MEMORY = 2
		};
	private:
		StorageType type_;
		bool cacheSizeGiven_;
		std::size_t cacheInBytes_;
		std::size_t cacheInGBytes_;

	public:

#ifdef HAVE_LEVELDB
		StorageConfig(const StorageType type = LEVELDB,
				const std::size_t cacheInBytes = 0,
				const std::size_t cacheInGBytes = 0) :
			type_(type), cacheInBytes_(cacheInBytes),
					cacheInGBytes_(cacheInGBytes_) {
#else
#ifdef HAVE_DB_CXX_H
			StorageConfig(const StorageType type = BERKELEY_DB,
					const std::size_t cacheInBytes = 0,
					const std::size_t cacheInGBytes = 0) :
			type_(type), cacheInBytes_(cacheInBytes),
			cacheInGBytes_(cacheInGBytes_) {
#endif
#endif
#ifndef HAVE_LEVELDB
			if (type_ == LEVELDB) {
				throw "Leveldb is not supported!";
			}
#endif
			if (cacheInGBytes == 0) {
				if (cacheInBytes == 0) {
					this->cacheSizeGiven_ = false;
				} else {
					this->cacheSizeGiven_ = true;
				}
			}
		}
		virtual ~StorageConfig() {
		}
		StorageType getType(void) const {
			return this->type_;
		}
		bool isCacheSizeGiven() const {
			return this->cacheSizeGiven_;
		}
		std::size_t getByteCacheSize() const {
			return this->cacheInBytes_;
		}
		std::size_t getGByteCacheSize() const {
			return this->cacheInGBytes_;
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
	Configuration(const BloomFilterConfig& bf,
			const std::string hashname = utils::CryptoHash::getDefaultName());
	Configuration(const TrieConfig& trie,
			const std::string hashname = utils::CryptoHash::getDefaultName());
	Configuration(const BloomFilterConfig& bf, const TrieConfig& trie,

	const std::string hashname = utils::CryptoHash::getDefaultName());
	Configuration(const SET_CONFIG config);
	virtual ~Configuration();
	std::string getHashFunction() const;
	const Configuration::BloomFilterConfig& getBloomFilter() const;
	const Configuration::TrieConfig& getTrie() const;
	const Configuration::StorageConfig& getStorage() const;
private:
	BloomFilterConfig bfConfig_;
	TrieConfig trieConfig_;
	StorageConfig storageConfig_;
};

}

}

#endif /* CONFIG_H_ */
