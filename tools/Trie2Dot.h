/*
 * Trie2Dot.h
 *
 *      Author: Till Lorentzen
 */

#ifndef TRIE2DOT_H_
#define TRIE2DOT_H_
#include <setsync/trie/KeyValueTrie.h>
#include <setsync/storage/KeyValueStorage.h>
#include <setsync/storage/BdbStorage.h>
#include <setsync/utils/CryptoHash.h>


class Trie2Dot {
private:
	Db * db1_;
	Db * db2_;
	setsync::utils::CryptoHash hash_;
	setsync::trie::KeyValueTrie * trie1_;
	setsync::trie::KeyValueTrie * trie2_;
	setsync::storage::AbstractKeyValueStorage * storage1;
	setsync::storage::AbstractKeyValueStorage * storage2;
public:
	Trie2Dot();
	virtual void out();
	virtual ~Trie2Dot();
};

#endif /* TRIE2DOT_H_ */
