/*
 * LevelDbTrie.h
 *
 *      Author: Till Lorentzen
 */

#ifndef LEVELDBTRIE_H_
#define LEVELDBTRIE_H_

#include "KeyValueTrie.h"

namespace trie {

class LevelDbTrie: public trie::AbstractKeyValueTrie {
public:
	LevelDbTrie(const utils::CryptoHash& hash);
	virtual ~LevelDbTrie();
};

}

#endif /* LEVELDBTRIE_H_ */
