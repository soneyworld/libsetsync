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
};

}
#endif /* MEMTRIE_H_ */
