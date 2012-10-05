/*
 * MemTrie.h
 *
 *      Author: Till Lorentzen
 */

#ifndef MEMTRIE_H_
#define MEMTRIE_H_

#include "Trie.h"
#include "setsync/sha1.h"
#include <stdint.h>

namespace trie {

class MemTrie;

class MemTrieNode {
private:
	Trie * trie_;
public:
	MemTrieNode(Trie * trie);
	virtual ~MemTrieNode();
	uint8_t commonPrefixSize(MemTrieNode * node);
	bool remove(const unsigned char* hash, bool performhash);
	void updateHash();
	MemTrieNode * parent_;
	MemTrieNode * smaller_;
	MemTrieNode * larger_;
	unsigned char * hash_;
	unsigned char * prefix_;
	uint8_t prefix_mask_;
	bool similar(unsigned char *a);
};

class MemTrie: public trie::Trie {
private:
	MemTrieNode * root_;
	void performHash();
	void clear(MemTrieNode * node);
public:
	MemTrie(const size_t hashsize = SHA_DIGEST_LENGTH);
	virtual ~MemTrie();
	bool add(const unsigned char * hash);
	bool add(const unsigned char * hash, bool performhash);
	bool remove(const unsigned char * hash);
	bool remove(const unsigned char * hash, bool performhash);
	bool contains(const unsigned char * hash) const;
	void clear(void);
};



}
#endif /* MEMTRIE_H_ */
