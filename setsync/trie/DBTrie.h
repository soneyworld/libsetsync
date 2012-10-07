/*
 * DBTrie.h
 *
 *      Author: Till Lorentzen
 */

#ifndef DBTRIE_H_
#define DBTRIE_H_

#include "Trie.h"
#include <db_cxx.h>

namespace trie {

class DBTrie: public trie::Trie {
public:
	DBTrie();
	virtual ~DBTrie();
	virtual bool add(const unsigned char * hash, bool performhash);
	virtual bool remove(const unsigned char * hash, bool performhash);
	virtual bool contains(const unsigned char * hash) const;
	virtual void clear(void);
};

}

#endif /* DBTRIE_H_ */
