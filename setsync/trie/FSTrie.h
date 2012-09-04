/*
 * FSTrie.h
 *
 *      Author: Till Lorentzen
 */

#ifndef FSTRIE_H_
#define FSTRIE_H_

#include "Trie.h"

namespace trie {

class FSTrie: public trie::Trie {
public:
	FSTrie();
	virtual ~FSTrie();
	bool insert(const unsigned char * hash);
	bool insert(const unsigned char * hash, bool performhash);
	bool erase(const unsigned char * hash);
	bool erase(const unsigned char * hash, bool performhash);
	void clear(void);
};

}

#endif /* FSTRIE_H_ */
