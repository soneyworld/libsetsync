/*
 * Set.hpp
 *
 *      Author: Till Lorentzen
 */

#ifndef SET_HPP_
#define SET_HPP_
#include "set.h"
#include <stdint.h>
#include <setsync/bloom/BloomFilter.h>
#include <setsync/trie/Trie.h>
#include <setsync/utils/CryptoHash.h>

namespace setsync {

class Set {
private:
	size_t maxSize_;
	bool hardMaximum_;
protected:
	void setSize(const size_t s);
	bloom::AbstractBloomFilter * bf_;
	trie::Trie * trie_;
	const utils::CryptoHash& hash_;
public:
	Set(const utils::CryptoHash& hash, const uint64_t maxSize = 1000,
			const bool hardMaximum = false);
	bool isEmpty() const;
	size_t getSize() const;
	size_t getMaximumSize() const;
	// Lookup
	virtual bool find(const unsigned char * key);
	virtual bool find(const char * str);
	virtual bool find(const std::string& str);

	// Modifiers
	virtual bool insert(const unsigned char * key);
	virtual bool insert(const char * str);
	virtual bool insert(const std::string& str);
	virtual bool erase(const unsigned char * key);
	virtual bool erase(const char * str);
	virtual bool erase(const std::string& str);
	virtual void clear();

	virtual ~Set();

};
}

#endif /* SET_HPP_ */
