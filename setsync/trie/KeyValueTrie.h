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
#include <setsync/DiffHandler.h>
#include <exception>
#include <vector>
#include <stdint.h>

namespace trie {

class KeyValueTrie;
class KeyValueRootNode;

class TrieNode {
	friend class KeyValueRootNode;
	friend class KeyValueTrie;
	friend class TrieValue;
#ifdef HAVE_LEVELDB
	friend class LevelDbTrieTest;
#endif
private:
	static const uint8_t HAS_PARENT;
	static const uint8_t HAS_CHILDREN;
	static const uint8_t DIRTY;
	static void unmarshall(TrieNode& target, const unsigned char * loadedValue, const std::size_t loadedSize);
	static void marshall(const TrieNode& source, const unsigned char * targetBuffer);
	static const std::size_t getMarshallBufferSize(const TrieNode& node);
	/// Reference to the key value storage where the node (should) exist
	setsync::storage::AbstractKeyValueStorage& storage_;
	/**
	 * Proofs, if the given nodes prefix is similar to this->prefix
	 * in the first this->prefix_mask count of bits.
	 * \param node to be checked
	 * \return true if similar, false if not
	 */
	bool similar(const TrieNode& node) const;
	/**
	 * Calculates the number of bits, which are common in this->prefix
	 * and other->prefix
	 * \param other node with its prefix to be checked
	 * \return common bits
	 */
	uint8_t commonPrefixSize(TrieNode& other) const;
	/// true, if this node has a parent
	bool hasParent_;
	/// true, if this node has children
	bool hasChildren_;
	/// true, if the hash of this node hasn't been updated since a new child has been added
	bool dirty_;
	/// The hash of this node
	unsigned char * hash;
	/// The hash of the parent of this node
	unsigned char * parent;
	/// The hash of the smaller child of this node
	unsigned char * smaller;
	/// The hash of the larger child of this node
	unsigned char * larger;
	/// The prefix of this node
	unsigned char * prefix;
	/// The size of the prefix
	uint8_t prefix_mask;

	const utils::CryptoHash& hashfunction_;

	const KeyValueTrie& trie_;
	/**
	 * Constructor of a node, which could exist in the given DB, or should be
	 * created to be saved in the given DB. The given hash could be the default
	 * hash and prefix value, if this node should be created
	 *
	 * \param db where this node should be used
	 * \param hash of the node, which should be created or loaded
	 * \param newone if true, a new node will be created, otherwise load the node with this hash from the given DB
	 */
	TrieNode(const KeyValueTrie& trie,
			const utils::CryptoHash& hashfunction,
			setsync::storage::AbstractKeyValueStorage& storage,
			const unsigned char * hash, bool newone = false);
	/**
	 * Sets the given parent as new parent and overwrites the old one.
	 * This method is in memory only! To save the changes to DB, call toDB()
	 *
	 * \param parent
	 */
	void setParent(const TrieNode& parent);
	/**
	 * Overwrites the smaller child of this node. If this node do not got children,
	 * an exception is thrown. This method do not update the hash
	 * of this node.
	 *
	 * \param smaller child
	 */
	void setSmaller(const TrieNode& smaller);
	/**
	 * Overwrites the larger child of this node. If this node do not got children,
	 * an exception is thrown. This method do not update the hash
	 * of this node.
	 *
	 * \param larger child
	 */
	void setLarger(const TrieNode& larger);
	/**
	 * Sets/overwrites the children of this node. This method do not update the hash
	 * of this node.
	 *
	 * \param smaller child
	 * \param larger child
	 */
	void setChildren(const TrieNode& smaller, const TrieNode& larger);
	/**
	 * Inserts the given node into the trie. This method also updates the hash of the
	 * parents and updates all links to the changed nodes.
	 *
	 * \param node to be inserted
	 */
	bool insert(TrieNode& node);
	/**
	 * Inserts the given node into the trie. This method updates the hash of the parents
	 * if performHash is set to true. Otherwise only the directly involved nodes will be
	 * updated and the parent will be marked as dirty.
	 *
	 * If performHash is true, the root of the trie will be changed after this method call
	 *
	 * \param node to be inserted
	 * \param performHash if true, parents will also be updated
	 * \return true if insert has been successful, false if the given node already exists
	 */
	bool insert(TrieNode& node, bool performHash);
	/**
	 * Erases the given node from trie. Also the hashes of the parents will be updated.
	 * The root of trie will be changed after a successful call
	 *
	 * \param node to be deleted
	 * \return true if successfully deleted, otherwise false
	 */
	bool erase(TrieNode& node);
	/**
	 * Erases the given node from trie. If performHash is true, also the hashes of the
	 * parents will be updated. If perfromHash is false, only the direct parent will
	 * be updated.
	 *
	 * \param node to be erased
	 * \param performHash set true to update all parents, false to set parent only as dirty
	 * \return true on successful deletion, otherwise false
	 */
	bool erase(TrieNode& node, bool performHash);
	/**
	 * Calculates a new hash, based on the hashes of the children hashes
	 */
	void updateHash();
	/**
	 * Deletes the entry of the DB with the hash of this node
	 * \return true if it has been deleted successfully
	 */
	bool deleteFromDb();
	/**
	 * \return true if the given node has the same hash as it is saved for the smaller child
	 */
	bool isEqualToSmaller(const TrieNode& node) const;
	/**
	 * \return true if the given node has the same hash as it is saved for the larger child
	 */
	bool isEqualToLarger(const TrieNode& node) const;
public:
	TrieNode(const KeyValueTrie& trie,
			const utils::CryptoHash& hashfunction,
			setsync::storage::AbstractKeyValueStorage& storage);
	/**
	 * Copies all member variables of other to the new instance
	 *
	 * \param other to be copied
	 */
	TrieNode(const TrieNode& other);
	/**
	 * \return true if node has children, otherwise false
	 */
	bool hasChildren() const;
	/**
	 * \return true if node has parent, otherwise false
	 */
	bool hasParent() const;
	/**
	 * A node is dirty, if a child has been changed and updateHash hasn't been
	 * called. So this->hash is not correct.
	 *
	 * \return true if hash has to be updated, otherwise false
	 */
	bool isDirty() const;
	/**
	 * To get the smaller child of this node, a db request for the this->smaller
	 * hash will be done and the result will be returned.
	 *
	 * \return the smaller child of this node
	 */
	TrieNode getSmaller() const;
	/**
	 * To get the larger child of this node, a db request for the this->larger
	 * hash will be done and the result will be returned.
	 *
	 * \return the larger child of this node
	 */
	TrieNode getLarger() const;
	/**
	 * To get the parent of this node, a db request for the this->parent
	 * hash will be done and the result will be returned.
	 *
	 * \return the parent of this node
	 */
	TrieNode getParent() const;
	bool isSmaller(const TrieNode& smaller) const;
	bool isLarger(const TrieNode& larger) const;
	/**
	 * Inserts a given hash to this node, or it's children and updates
	 * all parent nodes hashes.
	 *
	 * \param hash to be added to the Trie
	 * \return true on success, otherwise false
	 */
	bool insert(const unsigned char * hash);
	/**
	 * Inserts a given hash to this node, or it's children and updates
	 * the parents hash. If performHash is false, this method will only
	 * update the hash of the direct parent, not all parents.
	 *
	 * \param hash to be added to the Trie
	 * \return true on success, otherwise false
	 */
	bool insert(const unsigned char * hash, bool performHash);
	/**
	 * Deletes the given hash from the Trie, starting on this node.
	 * Also updates all parents hashes of this node.
	 *
	 * \param hash to be deleted.
	 * \return true on success, false otherwise
	 */
	bool erase(const unsigned char * hash);
	/**
	 * Deletes the given hash from the Trie, starting on this node.
	 * If performHash is false, only the direct parents hash will be
	 * updated. If performHash is true, all parents of the deleted hash
	 * will be updated.
	 *
	 * \param hash to be deleted
	 * \param performHash shows, if parents got to be updated
	 * \return true on success, false otherwise
	 */
	bool erase(const unsigned char * hash, bool performHash);
	/**
	 * Proofs, if the other node has got the same entries
	 *
	 * \param other node to check equality
	 * \return true, if both nodes are equal
	 */
	bool operator ==(const TrieNode& other) const;
	/**
	 * Proofs, if the other node has got a different entry
	 *
	 * \param other node to check for no equality
	 * \return true, if both nodes are different
	 */
	bool operator !=(const TrieNode& other) const;
	bool operator >(const TrieNode& other) const;
	bool operator <(const TrieNode& other) const;
	TrieNode& operator=(const TrieNode& rhs);
	/**
	 * \return dot code of this node and its children
	 */
	virtual std::string toString(const std::string nodePrefix) const;
	/**
	 * Saves the node to berkeley db. The member vars will be marshaled
	 * by using DBValue and the hash will be used as key for the db
	 */
	bool toDb();

	uint8_t getFlags() const;
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
private:
	/// The storage, which should be used
	setsync::storage::AbstractKeyValueStorage& storage_;
	/// The key of the <key,value> pair in the DB, where the root hash is saved as value
	static const char root_name[];
	/// Reference to the used cryptographic hash function
	const utils::CryptoHash& hashfunction_;
	/// Reference to the Trie, which should be used for operations
	const KeyValueTrie& trie_;
public:
	KeyValueRootNode(const KeyValueTrie& trie,
			const utils::CryptoHash& hashfunction,
			setsync::storage::AbstractKeyValueStorage& storage);
	/**
	 * Loads the hash of the DbNode, which is saved as root node. Throws a
	 * NoRootFoundException if no root is available on this storage.
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

class KeyValueTrie: public trie::Trie {
private:
	/// Instance of root node methods
	KeyValueRootNode * root_;
	/// Reference to the used key value storage
	setsync::storage::AbstractKeyValueStorage& storage_;
	/**
	 * Adds a cut through the subtree of the given root node.
	 * The numberOfNodes is the maximum number of nodes, to be
	 * added to the cut.
	 *
	 * \param root node of the requested subtree
	 * \param numberOfNodes to be added to the subtree
	 * \param innerNodes is a vector to save an inner node to the cut
	 * \param leafNodes is a vector to save leaf nodes to the cut
	 * \return the number of found nodes in the subtree
	 */
	size_t
			getSubTrie(const TrieNode& root, const size_t numberOfNodes,
					std::vector<TrieNode>& innerNodes,
					std::vector<TrieNode>& leafNodes);
public:
	KeyValueTrie(const utils::CryptoHash& hash,
			setsync::storage::AbstractKeyValueStorage& storage);
	virtual ~KeyValueTrie();
	/**
	 * \param hash to be added
	 * \param performhash
	 * \return true on success
	 */
	virtual bool add(const unsigned char * hash, bool performhash);
	/**
	 * \param hash to be removed
	 * \param performhash
	 * \return true on success
	 */
	virtual bool remove(const unsigned char * hash, bool performhash);
	/**
	 * \param hash to be checked
	 * \return true if the hash is available
	 */
	virtual enum TrieNodeType contains(const unsigned char * hash) const;
	/**
	 * Clears the complete Trie
	 */
	virtual void clear(void);
	/**
	 * \return true if both root elements hashes are equal
	 */
	virtual bool operator ==(const Trie& other) const;
	/**
	 * Generates a dump of the trie into a string. This string
	 * is in dot language. It can be used to draw a directed graph
	 * of the Trie
	 *
	 * \return dot string
	 */
	virtual std::string toString() const;
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
	 *
	 * \param subtrie
	 * \param length
	 * \param handler
	 */
	virtual void diff(const void * subtrie, const std::size_t length,
			setsync::AbstractDiffHandler& handler) const;
};

}

#endif /* KEYVALUETRIE_H_ */
