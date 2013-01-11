/*
 * Trie.h
 *
 *      Author: Till Lorentzen
 */

#ifndef TRIE_H_
#define TRIE_H_
#include <setsync/sync/Synchronization.h>
#include <cstring>
#include <string>
#include <setsync/crypto/CryptoHash.h>

namespace setsync {
namespace trie {
/**
 * A Trie can contain more than just the inserted hashes,
 * so the information, if a node is a leaf or an inner_node
 * can be specified with the following enumeration. If it
 * is not clear, which type of entry has been found, the
 * NOT_SPECIFIED flag can be used, but it should be avoided!
 * So the usage of NOT_SPECIFIED is deprecated!
 */
enum TrieNodeType {
	NOT_FOUND = 0, LEAF_NODE, INNER_NODE, NOT_SPECIFIED
};
/**
 *
 */
class Trie{
private:
	/// The number of the saved leafs, not the number of nodes!
	size_t size;
	/// Shows, if a rehashing should be performed
	bool needsHashPerforming;
protected:
	/// The used cryptographic hash algorithm
	const utils::CryptoHash& hash_;
	/**
	 * Sets the number of leaf nodes
	 *
	 * \param size is the new number of leaf nodes
	 */
	void setSize(const size_t size);
	/**
	 * Increments the number of leaf nodes by one
	 */
	void incSize();
	/**
	 * Decrements the number of leaf nodes by one
	 */
	void decSize();
	/**
	 * Proofs, if an insert or remove has been executed without
	 * all parents has been rehashed. This means, that the Trie
	 * has to be rehashed to update to the correct root hash.
	 *
	 * \return true if any element in the Trie is dirty
	 */
	bool isHashPerformingNedded();
	/**
	 * This method should be called after all nodes are in a
	 * clean state, so no node is marked as dirty.
	 */
	void setHashingPerformed();
public:
	Trie(const utils::CryptoHash& hash);
	/**
	 * This method requests the number of leaf nodes in the
	 * Trie. It gives read access to the private size variable.
	 *
	 * \return the number of leaf nodes
	 */
	size_t getSize() const;
	const utils::CryptoHash& getHash() const;
	/// Temporary memory for hashing two child nodes
	unsigned char * hashscratch;
	/**
	 * Takes the given char array as a string and adds a new
	 * entry in the Trie by using the cryptographic hash algorithm
	 * to calculate the hash digit to be saved as leaf node.
	 *
	 * \param str string to be hashed and its hash will be added
	 * \return true on success, false otherwise
	 */
	virtual bool add(const char * str);
	/**
	 * Takes the given char array as a string and adds a new
	 * entry in the Trie by using the cryptographic hash algorithm
	 * to calculate the hash digit to be saved as leaf node.
	 * If performhash is false, the bew node will only be inserted,
	 * but the hash of the parent nodes will not be updated.
	 *
	 * \param str string to be hashed and its hash will be added
	 * \param performhash if true, all nodes to the root will be updated
	 * \return true on success, false otherwise
	 */
	virtual bool add(const char * str, bool performhash);
	/**
	 * Takes the given std::string and adds a new
	 * entry in the Trie by using the cryptographic hash algorithm
	 * to calculate the hash digit to be saved as leaf node.
	 *
	 * \param str string to be hashed and its hash will be added
	 * \return true on success, false otherwise
	 */
	virtual bool add(const std::string& str);
	/**
	 * Takes the given std::string and adds a new
	 * entry in the Trie by using the cryptographic hash algorithm
	 * to calculate the hash digit to be saved as leaf node.
	 * If performhash is false, the bew node will only be inserted,
	 * but the hash of the parent nodes will not be updated.
	 *
	 * \param str string to be hashed and its hash will be added
	 * \param performhash if true, all nodes to the root will be updated
	 * \return true on success, false otherwise
	 */
	virtual bool add(const std::string& str, bool performhash);
	virtual bool add(const unsigned char * hash);
	virtual bool add(const unsigned char * hash, bool performhash) = 0;
	virtual bool remove(const char * str);
	virtual bool remove(const char * str, bool performhash);
	virtual bool remove(const std::string& str);
	virtual bool remove(const std::string& str, bool performhash);
	virtual bool remove(const unsigned char * hash);
	virtual bool remove(const unsigned char * hash, bool performhash) = 0;
	virtual enum TrieNodeType contains(const char * str) const;
	virtual enum TrieNodeType contains(const std::string& str) const;
	virtual enum TrieNodeType contains(const unsigned char * hash) const = 0;
	/**
	 * Deletes all nodes from the Trie, this can be different in
	 * various ways of implementations, if the underlying technology
	 * provides faster ways to free all allocated nodes together
	 */
	virtual void clear(void) = 0;
	virtual ~Trie();
	/**
	 * Checks, if both Trie have got exact the same hash key as root
	 *
	 * \return true if the trie instances have got same root
	 */
	virtual bool operator ==(const Trie& other) const = 0;
	/**
	 * Checks, if both Trie have got a different hash key as root
	 *
	 * \return true if the trie instances have got different root
	 */
	virtual bool operator !=(const Trie& other) const {
		return !this->operator ==(other);
	}
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
			const size_t buffersize) = 0;
	/**
	 * Copies the root of the trie into the given memory
	 *
	 * \param hash memory space, where the root should be copied to
	 * \return true if a root is available, false otherwise
	 *
	 */
	virtual bool getRoot(unsigned char * hash) = 0;
	/**
	 * Generates a string, containing a graph in the dot language.
	 * Only generates the Nodes and its properties, not the surrounding
	 * (directed) graph or a sub graph/sub cluster properties.
	 *
	 * \return dot string containing the whole trie
	 */
	virtual std::string toDotString() const;
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
	virtual std::string toDotString(const std::string nodePrefix) const = 0;
};
}
}

#endif /* TRIE_H_ */
