/*
 * DBTrie.h
 *
 *      Author: Till Lorentzen
 */

#ifndef DBTRIE_H_
#define DBTRIE_H_
#define MAXFIELD 20

#include "Trie.h"
#include <db_cxx.h>
#include <setsync/BerkeleyDBTableUserInterface.h>

namespace trie {

class DBValue {
public:
	DBValue();
	virtual ~DBValue() {
	}
	unsigned char parent[MAXFIELD];
	unsigned char smaller[MAXFIELD];
	unsigned char larger[MAXFIELD];
	unsigned char prefix[MAXFIELD];
	uint8_t prefix_mask;
};

class DbInnerNode {
public:
	DBValue value;
	unsigned char hash[MAXFIELD];
};

class DbNode {
private:
	static unsigned char hashscratch[MAXFIELD * 2];
	static unsigned char nullarray[MAXFIELD];
	static char root_name[];
	Db * db_;
	bool similar(const DbNode& node) const;
	uint8_t commonPrefixSize(DbNode& other) const;
	bool hasParent_;
	bool hasChildren_;
	bool dirty_;
	unsigned char hash[MAXFIELD];
	unsigned char parent[MAXFIELD];
	unsigned char smaller[MAXFIELD];
	unsigned char larger[MAXFIELD];
	unsigned char prefix[MAXFIELD];
	uint8_t prefix_mask;
	DbNode(Db * db, const unsigned char * hash);
	void setParent(DbNode& parent);
	void setSmaller(DbNode& smaller);
	void setLarger(DbNode& larger);
	bool insert(DbNode& node);
	bool insert(DbNode& node, bool performHash);
	bool erase(DbNode& node);
	bool erase(DbNode& node, bool performHash);
	void updateHash();
	DbNode getNode(const unsigned char * hash);
public:
	DbNode(Db * db);
	DbNode(const DbNode& other);
	bool hasChildren();
	bool hasParent();
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
	DbNode& operator=( const DbNode& rhs );
};

class DBTrie: public trie::Trie,
		public virtual berkeley::BerkeleyDBTableUserInferface {
private:
	Db * db_;
	DbInnerNode * root_;
	void getRootFromDB();
	void putRootToDB();
	bool insertHash(const unsigned char * hash);
	bool insertAtNode(const unsigned char * hash, DbInnerNode * currentnode,
			DbInnerNode * newnode);
	bool similar(unsigned char *a, unsigned char *b, uint8_t count);
	bool loadNode(DbInnerNode * target, const unsigned char * hash);
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
