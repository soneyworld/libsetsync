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
	static const uint8_t HAS_PARENT;
	static const uint8_t HAS_CHILDREN;
	static const uint8_t DIRTY;
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
public:
	DbNode(const DbNode& other);
	bool hasChildren() const;
	bool hasParent() const;
	bool isDirty() const;
	DbNode getSmaller();
	DbNode getLarger();
	DbNode getParent();
	bool insert(const unsigned char * hash);
	bool insert(const unsigned char * hash, bool performHash);
	bool erase(const unsigned char * hash);
	bool erase(const unsigned char * hash, bool performHash);
	bool operator ==(const DbNode& other) const;
	bool operator !=(const DbNode& other) const;
	bool operator >(const DbNode& other) const;
	bool operator <(const DbNode& other) const;
	DbNode& operator=(const DbNode& rhs);
	bool toDb();
};

class DbRootNode {
	friend class DbRootNodeTest;
private:
	Db * db_;
	static const char root_name[];
	unsigned char hash[HASHSIZE];
public:
	DbRootNode(Db * db);
	DbRootNode(Db * db, const unsigned char * hash);
	DbRootNode(Db * db, const char * key);
	DbRootNode(Db * db, const std::string& key);
	void saveToDB();
	DbNode getRootNode();
};

class DBTrie: public trie::Trie,
		public virtual berkeley::BerkeleyDBTableUserInferface {
private:
	Db * db_;
	DbRootNode * root_;
	void getRootFromDB();
	void putRootToDB();
	//	bool insertHash(const unsigned char * hash, bool performhash);
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
};

}

#endif /* DBTRIE_H_ */
