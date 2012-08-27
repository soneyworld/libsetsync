/*
 * Trie.h
 *
 *      Author: Till Lorentzen
 */

#ifndef TRIE_H_
#define TRIE_H_

#include <cstring>

namespace trie {

class Trie {
private:
	size_t size;
public:
	Trie();
	size_t getSize();
	virtual ~Trie();
};

class FSTrie: public Trie {
public:
	FSTrie();
	virtual ~FSTrie();
};

}

#endif /* TRIE_H_ */
