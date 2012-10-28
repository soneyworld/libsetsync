/*
 * Set.hpp
 *
 *      Author: Till Lorentzen
 */

#ifndef SET_HPP_
#define SET_HPP_
#include "set.h"
#include <stdint.h>
#include "setsync/bloom/BloomFilter.h"
#include "setsync/trie/Trie.h"
#include "setsync/sha1.h"
namespace setsync {

class Set {
private:
	size_t maxSize_;
	size_t hashSize_;
	bool hardMaximum_;
protected:
	void setSize(const size_t s);
	bloom::AbstractBloomFilter * bf_;
	trie::Trie * trie_;
public:
	Set(const uint64_t maxSize = 1000, const bool hardMaximum = false,
			const std::size_t hashsize = SHA_DIGEST_LENGTH);
	bool isEmpty() const;
	size_t getSize() const;
	size_t getMaximumSize() const;

	// Lookup
	virtual bool find(const unsigned char * key);
	virtual bool find(const char * key);
	virtual bool find(const std::string key);

	// Modifiers
	virtual bool insert(const unsigned char * key);
	virtual bool insert(const char * key);
	virtual bool insert(const std::string key);
	virtual bool erase(const unsigned char * key);
	virtual bool erase(const char * key);
	virtual bool erase(const std::string key);
	virtual void clear();

	virtual ~Set();

};
}

#endif /* SET_HPP_ */
