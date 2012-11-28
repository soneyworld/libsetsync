/*
 * SQLiteTrie.h
 *
 *      Author: Till Lorentzen
 */

#ifndef SQLITETRIE_H_
#define SQLITETRIE_H_
#include "Trie.h"
#include <setsync/utils/CryptoHash.h>

namespace trie {
class SQLiteTrie: public Trie {
public:
	SQLiteTrie(const utils::CryptoHash& hash);
	virtual ~SQLiteTrie();
	bool add(const unsigned char * hash);
	bool add(const unsigned char * hash, bool performhash);
	bool remove(const unsigned char * hash);
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
	 * (directed) graph or a sub graph/sub cluster properties.
	 *
	 * \return dot string containing the whole trie
	 */
	virtual std::string toDotString(const std::string nodePrefix) const;
};

}
#endif /* SQLITETRIE_H_ */
