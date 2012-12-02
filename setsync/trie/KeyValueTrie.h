/*
 * KeyValueTrie.h
 *
 *      Author: Till Lorentzen
 */

#ifndef KEYVALUETRIE_H_
#define KEYVALUETRIE_H_

#include "Trie.h"

namespace trie {

class TrieNode {

};

class KeyValueStoreException : public std::exception{

};

class AbstractKeyValueTrie: public trie::Trie {
protected:

public:
	AbstractKeyValueTrie(const utils::CryptoHash& hash);
	virtual ~AbstractKeyValueTrie();
};

}

#endif /* KEYVALUETRIE_H_ */
