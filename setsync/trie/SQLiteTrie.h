/*
 * SQLiteTrie.h
 *
 *      Author: Till Lorentzen
 */

#ifndef SQLITETRIE_H_
#define SQLITETRIE_H_
#include "Trie.h"


namespace trie{
class SQLiteTrie: public Trie {
public:
	SQLiteTrie();
	virtual ~SQLiteTrie();
	bool add(const unsigned char * hash);
	bool add(const unsigned char * hash, bool performhash);
	bool remove(const unsigned char * hash);
	bool remove(const unsigned char * hash, bool performhash);
	bool contains(const unsigned char * hash) const;
	void clear(void);
	virtual bool operator ==(const Trie& other) const;
};

}
#endif /* SQLITETRIE_H_ */
