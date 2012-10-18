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

class DbTrieException: public std::exception {
private:
	std::string what_;
public:
	DbTrieException(const char * message) throw () {
		what_ = std::string(message);
	}
	virtual ~DbTrieException() throw () {
	}
	virtual const char* what() {
		return what_.c_str();
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

class DbInnerNode {
public:
	DBValue value;
	unsigned char hash[HASHSIZE];
};

class DbNode {
	friend class DbRootNode;
	friend class DBValue;
	friend class DbNodeTest;
	friend class DBTrie;
private:
	static unsigned char hashscratch[HASHSIZE * 2];
	static unsigned char nullarray[HASHSIZE];
	Db * db_;
	bool similar(const DbNode& node) const;
	uint8_t commonPrefixSize(DbNode& other) const;
	bool hasParent_;
	bool hasChildren_;
	bool dirty_;
	unsigned char hash[HASHSIZE];
	unsigned char parent[HASHSIZE];
	unsigned char smaller[HASHSIZE];
	unsigned char larger[HASHSIZE];
	unsigned char prefix[HASHSIZE];
	uint8_t prefix_mask;
	DbNode(Db * db, const unsigned char * hash, bool newone = false);
	void setParent(const DbNode& parent);
	void setSmaller(const DbNode& smaller);
	void setLarger(const DbNode& larger);
	void setChildren(const DbNode& smaller, const DbNode& larger);
	bool insert(DbNode& node);
	bool insert(DbNode& node, bool performHash);
	bool erase(DbNode& node);
	bool erase(DbNode& node, bool performHash);
	void updateHash();
	bool deleteFromDb();
	bool isEqualToSmaller(const DbNode& node) const;
	bool isEqualToLarger(const DbNode& node) const;
public:
	DbNode(Db * db);
	DbNode(const DbNode& other);
	bool hasChildren() const;
	bool hasParent() const;
	bool isDirty() const;
	DbNode getSmaller() const;
	DbNode getLarger() const;
	DbNode getParent() const;
	bool insert(const unsigned char * hash);
	bool insert(const unsigned char * hash, bool performHash);
	bool erase(const unsigned char * hash);
	bool erase(const unsigned char * hash, bool performHash);
	bool operator ==(const DbNode& other) const;
	bool operator !=(const DbNode& other) const;
	bool operator >(const DbNode& other) const;
	bool operator <(const DbNode& other) const;
	DbNode& operator=(const DbNode& rhs);
	virtual std::string toString() const;
	bool toDb();
};

class DbRootNode {
	friend class DbRootNodeTest;
private:
	Db * db_;
	static const char root_name[];
public:
	DbRootNode(Db * db);
	DbNode get() const;
	void set(const unsigned char * hash);
	void del();
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
