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
	bool insert(const unsigned char * hash);
	bool insert(const unsigned char * hash, bool performhash);
	bool erase(const unsigned char * hash);
	bool erase(const unsigned char * hash, bool performhash);
	void clear(void);
};

}
#endif /* SQLITETRIE_H_ */
