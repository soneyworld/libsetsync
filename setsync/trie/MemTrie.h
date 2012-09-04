/*
 * MemTrie.h
 *
 *      Author: Till Lorentzen
 */

#ifndef MEMTRIE_H_
#define MEMTRIE_H_

#include "Trie.h"

namespace trie {

class MemTrie: public trie::Trie {
public:
	MemTrie();
	virtual ~MemTrie();
	bool insert(const unsigned char * hash);
	bool insert(const unsigned char * hash, bool performhash);
	bool erase(const unsigned char * hash);
	bool erase(const unsigned char * hash, bool performhash);
	void clear(void);
};

}
#endif /* MEMTRIE_H_ */
