/*
 * Set.hpp
 *
 *      Author: Till Lorentzen
 */

#ifndef SET_HPP_
#define SET_HPP_
#include "set.h"
#include <stdint.h>
#include <setsync/storage/KeyValueStorage.h>
#include <setsync/bloom/KeyValueCountingBloomFilter.h>
#include <setsync/index/KeyValueIndex.h>
#include <setsync/trie/KeyValueTrie.h>
#include <setsync/utils/CryptoHash.h>
#include <setsync/SyncStrategy.h>
#include <setsync/config/Configuration.h>

namespace setsync {

class Set {
private:
	size_t maxSize_;
	bool hardMaximum_;
	const utils::CryptoHash hash_;
	const config::Configuration& config_;
	bloom::KeyValueCountingBloomFilter * bf_;
	trie::KeyValueTrie * trie_;
	index::KeyValueIndex * index_;
	setsync::storage::AbstractKeyValueStorage * bfStorage_;
	setsync::storage::AbstractKeyValueStorage * trieStorage_;
	setsync::storage::AbstractKeyValueStorage * indexStorage_;
protected:
	void setSize(const size_t s);
public:
	Set(const config::Configuration& config);
	bool isEmpty() const;
	size_t getSize() const;
	size_t getMaximumSize() const;
	// Lookup for existence
	virtual bool find(const unsigned char * key);
	virtual bool find(const char * str);
	virtual bool find(const std::string& str);
	virtual bool find(const void * data, const std::size_t length);

	// Modifiers
	virtual bool insert(const unsigned char * key);
	virtual bool insert(const char * str);
	virtual bool insert(const std::string& str);
	virtual bool insert(const void * data, const std::size_t length);
	virtual bool erase(const unsigned char * key);
	virtual bool erase(const char * str);
	virtual bool erase(const std::string& str);
	virtual bool erase(const void * data, const std::size_t length);
	virtual void clear();
	// Synchronization Strategy
	virtual SynchronizationStrategy createSyncStrategy(void);

	virtual ~Set();

};
}

#endif /* SET_HPP_ */
