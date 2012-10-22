/*
 * DBTrie.h
 *
 *      Author: Till Lorentzen
 */

#ifndef DBTRIE_H_
#define DBTRIE_H_
#define HASHSIZE 20

#include "Trie.h"
#include <db_cxx.h>
#include <setsync/BerkeleyDBTableUserInterface.h>
#include <exception>

namespace trie {

class DbRootNode;
class DbNode;
class DBTrie;
/**
 * This exception should be thrown, if a consistency failure happens
 */
class DbTrieException: public std::exception {
private:
	/// Message returned by calling what()
	std::string what_;
public:
	/**
	 * \param message Consistency fail message
	 */
	DbTrieException(const char * message) throw () {
		what_ = std::string(message);
	}
	virtual ~DbTrieException() throw () {
	}
	/**
	 * \return what consistency problem exists
	 */
	virtual const char* what() {
		return what_.c_str();
	}
};
/**
 * This exception should be thrown, if the root node of the trie
 * hasn't been found on the Berkeley DB
 */
class DbNoRootFoundException: public DbTrieException {
public:
	DbNoRootFoundException() throw () :
		DbTrieException("No root found") {
	}
	virtual ~DbNoRootFoundException() throw () {
	}
};

class DBValue {
public:
	DBValue(const DbNode& toSave);
	DBValue(void * toLoad);
	static const uint8_t HAS_PARENT;
	static const uint8_t HAS_CHILDREN;
	static const uint8_t DIRTY;
	static const std::size_t getBufferSize();
	static void marshall(void * target, const DBValue& toBeMarshalled);
	bool isDirty() const;
	bool hasChildren() const;
	bool hasParent() const;
	DBValue(const uint8_t flags = 0);
	virtual ~DBValue() {
	}
	unsigned char parent[HASHSIZE];
	unsigned char smaller[HASHSIZE];
	unsigned char larger[HASHSIZE];
	unsigned char prefix[HASHSIZE];
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
	friend class DBTrie;
private:
	/**
	 * field to build the new hash of a node
	 * is only used by updateHash function
	 */
	static unsigned char hashscratch[HASHSIZE * 2];
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
	unsigned char hash[HASHSIZE];
	/// The hash of the parent of this node
	unsigned char parent[HASHSIZE];
	/// The hash of the smaller child of this node
	unsigned char smaller[HASHSIZE];
	/// The hash of the larger child of this node
	unsigned char larger[HASHSIZE];
	/// The prefix of this node
	unsigned char prefix[HASHSIZE];
	/// The size of the prefix
	uint8_t prefix_mask;
	/**
	 * Constructor of a node, which could exist in the given DB, or should be
	 * created to be saved in the given DB. The given hash could be the default
	 * hash and prefix value, if this node should be created
	 *
	 * \param db where this node should be used
	 * \param hash of the node, which should be created or loaded
	 * \param newone if true, a new node will be created, otherwise load the node with this hash from the given DB
	 */
	DbNode(Db * db, const unsigned char * hash, bool newone = false);
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
	DbNode(Db * db);
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
	bool insert(const unsigned char * hash);
	bool insert(const unsigned char * hash, bool performHash);
	bool erase(const unsigned char * hash);
	bool erase(const unsigned char * hash, bool performHash);
	bool operator ==(const DbNode& other) const;
	bool operator !=(const DbNode& other) const;
	bool operator >(const DbNode& other) const;
	bool operator <(const DbNode& other) const;
	DbNode& operator=(const DbNode& rhs);
	/**
	 * \return dot code of this node and its children
	 */
	virtual std::string toString() const;
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
public:
	/**
	 * Initializes this class with the given pointer to the Berkeley DB, which should
	 * be used to load/save the root of the trie. The constructor do not make any
	 * Berkeley db calls.
	 *
	 * \param db pointer to berkeley db to be used
	 */
	DbRootNode(Db * db);
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
	virtual std::string toString() const;
};

class DBTrie: public trie::Trie,
		public virtual berkeley::BerkeleyDBTableUserInferface {
private:
	Db * db_;
	DbRootNode root_;
public:
	DBTrie(Db * db, const size_t hashsize = SHA_DIGEST_LENGTH);
	virtual ~DBTrie();
	virtual bool add(const unsigned char * hash, bool performhash);
	virtual bool remove(const unsigned char * hash, bool performhash);
	virtual bool contains(const unsigned char * hash) const;
	virtual void clear(void);
	static const char * getLogicalDatabaseName();
	static const DBTYPE getTableType();
	virtual bool operator ==(const Trie& other) const;
	virtual std::string toString() const;
};

}

#endif /* DBTRIE_H_ */
