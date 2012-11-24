/*
 * FSTrie.h
 *
 *      Author: Till Lorentzen
 */

#ifndef FSTRIE_H_
#define FSTRIE_H_

#include "Trie.h"

namespace trie {

class FSTrie: public trie::Trie {
public:
	FSTrie(const utils::CryptoHash& hash);
	virtual ~FSTrie();
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
};

}

#endif /* FSTRIE_H_ */
