/*
 * LevelDbTrie.cpp
 *
 *      Author: Till Lorentzen
 */

#include "LevelDbTrie.h"

namespace trie {

LevelDbTrie::LevelDbTrie(const utils::CryptoHash& hash) :
	AbstractKeyValueTrie(hash) {

}

LevelDbTrie::~LevelDbTrie() {
}

}
