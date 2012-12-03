/*
 * DBTrie.h
 *
 *      Author: Till Lorentzen
 */

#ifndef DBTRIE_H_
#define DBTRIE_H_

#include "Trie.h"
#include <db_cxx.h>
#include <setsync/BdbTableUser.h>
#include <exception>
#include <vector>
#include <setsync/DiffHandler.h>

namespace trie {

class DbRootNode;
class DbNode;
class BdbTrie;

class DBValue {
private:
	std::size_t hashsize;
public:
	DBValue(const DbNode& toSave);
	DBValue(void * toLoad, const std::size_t hashsize);
	static const uint8_t HAS_PARENT;
	static const uint8_t HAS_CHILDREN;
	static const uint8_t DIRTY;
	static const std::size_t getBufferSize(const std::size_t hashsize);
	void marshall(void * target);
	bool isDirty() const;
	bool hasChildren() const;
	bool hasParent() const;
	DBValue(const std::size_t hashsize, const uint8_t flags = 0);
	virtual ~DBValue();
	unsigned char * parent;
	unsigned char * smaller;
	unsigned char * larger;
	unsigned char * prefix;
	uint8_t prefix_mask;
	uint8_t flags;
};
/**
 * DbNode represents a node of the trie in the Berkeley DB.
 * It contains its parent and its children. It also provides methods to
 * insert/erase a hash. There is also a method to save the actual values
 * of this node to the given Berkeley DB.
 */
class DbNode {
	friend class DbRootNode;
	friend class DBValue;
	friend class DbNodeTest;
	friend class BdbTrie;
	friend class BdbTrieTest;
private:
	/// Pointer to the Berkeley DB where the node (should) exist
	Db * db_;
	/**
	 * Proofs, if the given nodes prefix is similar to this->prefix
	 * in the first this->prefix_mask count of bits.
	 * \param node to be checked
	 * \return true if similar, false if not
	 */
	bool similar(const DbNode& node) const;
	/**
	 * Calculates the number of bits, which are common in this->prefix
	 * and other->prefix
	 * \param other node with its prefix to be checked
	 * \return common bits
	 */
	uint8_t commonPrefixSize(DbNode& other) const;
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

	const BdbTrie& trie_;
	/**
	 * Constructor of a node, which could exist in the given DB, or should be
	 * created to be saved in the given DB. The given hash could be the default
	 * hash and prefix value, if this node should be created
	 *
	 * \param db where this node should be used
	 * \param hash of the node, which should be created or loaded
	 * \param newone if true, a new node will be created, otherwise load the node with this hash from the given DB
	 */
	DbNode(const BdbTrie& trie, const utils::CryptoHash& hashfunction, Db * db,
			const unsigned char * hash, bool newone = false);
	/**
	 * Sets the given parent as new parent and overwrites the old one.
	 * This method is in memory only! To save the changes to DB, call toDB()
	 *
	 * \param parent
	 */
	void setParent(const DbNode& parent);
	/**
	 * Overwrites the smaller child of this node. If this node do not got children,
	 * an exception is thrown. This method do not update the hash
	 * of this node.
	 *
	 * \param smaller child
	 */
	void setSmaller(const DbNode& smaller);
	/**
	 * Overwrites the larger child of this node. If this node do not got children,
	 * an exception is thrown. This method do not update the hash
	 * of this node.
	 *
	 * \param larger child
	 */
	void setLarger(const DbNode& larger);
	/**
	 * Sets/overwrites the children of this node. This method do not update the hash
	 * of this node.
	 *
	 * \param smaller child
	 * \param larger child
	 */
	void setChildren(const DbNode& smaller, const DbNode& larger);
	/**
	 * Inserts the given node into the trie. This method also updates the hash of the
	 * parents and updates all links to the changed nodes.
	 *
	 * \param node to be inserted
	 */
	bool insert(DbNode& node);
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
	bool insert(DbNode& node, bool performHash);
	/**
	 * Erases the given node from trie. Also the hashes of the parents will be updated.
	 * The root of trie will be changed after a successful call
	 *
	 * \param node to be deleted
	 * \return true if successfully deleted, otherwise false
	 */
	bool erase(DbNode& node);
	/**
	 * Erases the given node from trie. If performHash is true, also the hashes of the
	 * parents will be updated. If perfromHash is false, only the direct parent will
	 * be updated.
	 *
	 * \param node to be erased
	 * \param performHash set true to update all parents, false to set parent only as dirty
	 * \return true on successful deletion, otherwise false
	 */
	bool erase(DbNode& node, bool performHash);
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
	bool isEqualToSmaller(const DbNode& node) const;
	/**
	 * \return true if the given node has the same hash as it is saved for the larger child
	 */
	bool isEqualToLarger(const DbNode& node) const;
public:
	DbNode(const BdbTrie& trie, const utils::CryptoHash& hashfunction, Db * db);
	/**
	 * Copies all member variables of other to the new instance
	 *
	 * \param other to be copied
	 */
	DbNode(const DbNode& other);
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
	DbNode getSmaller() const;
	/**
	 * To get the larger child of this node, a db request for the this->larger
	 * hash will be done and the result will be returned.
	 *
	 * \return the larger child of this node
	 */
	DbNode getLarger() const;
	/**
	 * To get the parent of this node, a db request for the this->parent
	 * hash will be done and the result will be returned.
	 *
	 * \return the parent of this node
	 */
	DbNode getParent() const;
	bool isSmaller(const DbNode& smaller) const;
	bool isLarger(const DbNode& larger) const;
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
	bool operator ==(const DbNode& other) const;
	/**
	 * Proofs, if the other node has got a different entry
	 *
	 * \param other node to check for no equality
	 * \return true, if both nodes are different
	 */
	bool operator !=(const DbNode& other) const;
	bool operator >(const DbNode& other) const;
	bool operator <(const DbNode& other) const;
	DbNode& operator=(const DbNode& rhs);
	/**
	 * \return dot code of this node and its children
	 */
	virtual std::string toString(const std::string nodePrefix) const;
	/**
	 * Saves the node to berkeley db. The member vars will be marshaled
	 * by using DBValue and the hash will be used as key for the db
	 */
	bool toDb();
};
/**
 * Every trie must have a root node. This class is used to save/load a given hash
 * as the link to the root DbNode of the trie.
 */
class DbRootNode {
	friend class DbRootNodeTest;
private:
	/// The pointer to the Berkeley DB, which should be used
	Db * db_;
	/// The key of the <key,value> pair in the DB, where the root hash is saved as value
	static const char root_name[];
	/// Reference to the used cryptographic hash function
	const utils::CryptoHash& hashfunction_;
	/// Reference to the DBTrie, which should be used for operations
	const BdbTrie& trie_;
public:
	/**
	 * Initializes this class with the given pointer to the Berkeley DB, which should
	 * be used to load/save the root of the trie. The constructor do not make any
	 * Berkeley db calls.
	 *
	 * \param db pointer to berkeley db to be used
	 */
	DbRootNode(const BdbTrie& trie, const utils::CryptoHash& hashfunction,
			Db * db);
	/**
	 * Loads the hash of the DbNode, which is saved as root node. Throws a
	 * DbNoRootFoundException if no root is available on this db.
	 *
	 * \return the found root DbNode of the trie
	 */
	DbNode get() const;
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
/**
 * DBTrie instances are using a berkeley db to save a Trie data
 * structure with <key,value> pairs. The hash of a node will be
 * saved as key and the value are the other data of a Trie node.
 */
class BdbTrie: public trie::Trie,
		public virtual berkeley::AbstractBdbTableUser {
	friend class DbNode;
private:
	/// Pointer to the used berkeley DB
	Db * db_;
	/// Instance of root node methods
	DbRootNode root_;
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
	size_t getSubTrie(const DbNode& root, const size_t numberOfNodes,
			std::vector<DbNode>& innerNodes, std::vector<DbNode>& leafNodes);
public:
	/**
	 * Creates a new DBTrie instance which uses the given cryptographic
	 * hash algorithm. The given pointer to a berkeley DB will be used
	 * to operate on the database.
	 */
	BdbTrie(const utils::CryptoHash& hash, Db * db);
	/**
	 * Destroys the instance but doesn't destroy the database. It also
	 * don't close the database, this has to be done after this call.
	 */
	virtual ~BdbTrie();
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
	 * \return the name of the database
	 */
	static const char * getLogicalDatabaseName();
	/**
	 * \return the type of the table, used for this Trie
	 */
	static const DBTYPE getTableType();
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

#endif /* DBTRIE_H_ */
