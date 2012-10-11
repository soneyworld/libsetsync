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

class DBNode {
public:
	DBValue value;
	unsigned char hash[MAXFIELD];
};

class DBTrie: public trie::Trie,
		public virtual berkeley::BerkeleyDBTableUserInferface {
private:
	Db * db_;
	DBNode * root_;
	void getRootFromDB();
	void putRootToDB();
public:
	DBTrie(Db * db, const size_t hashsize = SHA_DIGEST_LENGTH);
	virtual ~DBTrie();
	virtual bool add(const unsigned char * hash, bool performhash);
	virtual bool remove(const unsigned char * hash, bool performhash);
	virtual bool contains(const unsigned char * hash) const;
	virtual void clear(void);
	static const char * getLogicalDatabaseName();
	static const DBTYPE getTableType();
};

}

#endif /* DBTRIE_H_ */
