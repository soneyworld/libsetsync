/*
 * KeyValueTrie.h
 *
 *      Author: Till Lorentzen
 */

#ifndef KEYVALUETRIE_H_
#define KEYVALUETRIE_H_

#include "Trie.h"
#include <setsync/storage/KeyValueStorage.h>
#include <setsync/trie/TrieException.h>

namespace trie {

class AbstractKeyValueTrie;
class TrieNode {

};

class KeyValueStoreException: public TrieException {
	KeyValueStoreException(const char * message) :
		TrieException(message) {
	}
	KeyValueStoreException(const std::string& message) :
		TrieException(message) {
	}
};

/**
 * Every trie must have a root node. This class is used to save/load a given hash
 * as the link to the root DbNode of the trie.
 */
class KeyValueRootNode {
	friend class DbRootNodeTest;
private:
	/// The storage, which should be used
	setsync::storage::AbstractKeyValueStorage& storage_;
	/// The key of the <key,value> pair in the DB, where the root hash is saved as value
	static const char root_name[];
	/// Reference to the used cryptographic hash function
	const utils::CryptoHash& hashfunction_;
	/// Reference to the Trie, which should be used for operations
	const AbstractKeyValueTrie& trie_;
public:
	/**
	 * Initializes this class with the given pointer to the Berkeley DB, which should
	 * be used to load/save the root of the trie. The constructor do not make any
	 * Berkeley db calls.
	 *
	 * \param db pointer to berkeley db to be used
	 */
	KeyValueRootNode(const AbstractKeyValueTrie& trie, const utils::CryptoHash& hashfunction);
	/**
	 * Loads the hash of the DbNode, which is saved as root node. Throws a
	 * DbNoRootFoundException if no root is available on this db.
	 *
	 * \return the found root DbNode of the trie
	 */
	TrieNode get() const;
	/**
	 * Sets/overwrites the saved hash, which corresponds to a DbNode hash
	 *
	 * \param hash to be saved as new root
	 */
	void set(const unsigned char * hash);
	/**
	 * Deletes the root of the trie
	 */
	void del();
	/**
	 * \return dot string
	 */
	virtual std::string toString(const std::string nodePrefix) const;
};

class AbstractKeyValueTrie: public trie::Trie {
private:
	setsync::storage::AbstractKeyValueStorage& storage_;
public:
	AbstractKeyValueTrie(const utils::CryptoHash& hash,
			setsync::storage::AbstractKeyValueStorage& storage);
	virtual ~AbstractKeyValueTrie();
};

}

#endif /* KEYVALUETRIE_H_ */
