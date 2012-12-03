/*
 * LevelDbTrie.cpp
 *
 *      Author: Till Lorentzen
 */

#include "LevelDbTrie.h"

namespace trie {

LevelDbTrie::LevelDbTrie(const utils::CryptoHash& hash, setsync::storage::LevelDbStorage& storage) :
	AbstractKeyValueTrie(hash, storage) {

}

LevelDbTrie::~LevelDbTrie() {
}

}
