/*
 * MemTrie.h
 *
 *      Author: Till Lorentzen
 */

#ifndef MEMTRIE_H_
#define MEMTRIE_H_

#include "Trie.h"
#include <setsync/utils/CryptoHash.h>
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
	MemTrie(const utils::CryptoHash& hash);
	virtual ~MemTrie();
	bool add(const unsigned char * hash, bool performhash);
	bool remove(const unsigned char * hash, bool performhash);
	enum TrieNodeType contains(const unsigned char * hash) const;
	void clear(void);
	virtual bool operator ==(const Trie& other) const;

	/**
	 * Copies a subtrie into the given buffer with a maximum size
	 * of buffersize. The root of the subtrie must be passed by the
	 * first argument.
	 *
	 * \param hash of the root of the requested subtrie
	 * \param buffer where the subtrie will by copied to
	 * \param buffersize for the subtrie
	 * \return used buffer size in bytes
	 */
	virtual size_t getSubTrie(const unsigned char * hash, void * buffer,
			const size_t buffersize);
	/**
	 * Copies the root of the trie into the given memory
	 *
	 * \param hash memory space, where the root should be copied to
	 * \return true if a root is available, false otherwise
	 *
	 */
	virtual bool getRoot(unsigned char * hash);

	/**
	 * Generates a string, containing a graph in the dot language.
	 * Only generates the Nodes and its properties, not the surrounding
	 * (directed) graph or a sub graph/sub cluster properties. The given
	 * prefix is used as prefix for the dot nodes. It can be useful for
	 * different tries in the same dot file.
	 *
	 * \param nodePrefix of the dot nodes
	 * \return dot string containing the whole trie
	 */
	virtual std::string toDotString(const std::string nodePrefix) const;
};

}
#endif /* MEMTRIE_H_ */
