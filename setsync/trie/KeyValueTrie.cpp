/*
 * KeyValueTrie.cpp
 *
 *      Author: Till Lorentzen
 */

#include "KeyValueTrie.h"

namespace trie {

AbstractKeyValueTrie::AbstractKeyValueTrie(const utils::CryptoHash& hash,
		setsync::storage::AbstractKeyValueStorage& storage) :
	Trie(hash), storage_(storage) {

}

AbstractKeyValueTrie::~AbstractKeyValueTrie() {
}

}
