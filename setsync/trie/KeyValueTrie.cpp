/*
 * KeyValueTrie.cpp
 *
 *      Author: Till Lorentzen
 */

#include "KeyValueTrie.h"

namespace trie {

AbstractKeyValueTrie::AbstractKeyValueTrie(const utils::CryptoHash& hash) :
	Trie(hash) {
	// TODO Auto-generated constructor stub

}

AbstractKeyValueTrie::~AbstractKeyValueTrie() {
	// TODO Auto-generated destructor stub
}

}
