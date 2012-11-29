/*
 * Trie2Dot.h
 *
 *      Author: Till Lorentzen
 */

#ifndef TRIE2DOT_H_
#define TRIE2DOT_H_
#include <setsync/trie/BdbTrie.h>
#include <setsync/utils/CryptoHash.h>


class Trie2Dot {
private:
	Db * db1_;
	Db * db2_;
	utils::CryptoHash hash_;
	trie::BdbTrie * trie1_;
	trie::BdbTrie * trie2_;
public:
	Trie2Dot();
	virtual void out();
	virtual ~Trie2Dot();
};

#endif /* TRIE2DOT_H_ */
