/*
 * KeyValueTrie.h
 *
 *      Author: Till Lorentzen
 */

#ifndef KEYVALUETRIE_H_
#define KEYVALUETRIE_H_

#include "Trie.h"
#include <setsync/storage/KeyValueStorage.h>

namespace trie {

class TrieNode {

};

class KeyValueStoreException : public std::exception{

};

class AbstractKeyValueTrie: public trie::Trie {
private:
	setsync::storage::AbstractKeyValueStorage& storage_;
public:
	AbstractKeyValueTrie(const utils::CryptoHash& hash, setsync::storage::AbstractKeyValueStorage& storage);
	virtual ~AbstractKeyValueTrie();
};

}

#endif /* KEYVALUETRIE_H_ */
