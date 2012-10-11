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
	bool add(const unsigned char * hash);
	bool add(const unsigned char * hash, bool performhash);
	bool remove(const unsigned char * hash);
	bool remove(const unsigned char * hash, bool performhash);
	bool contains(const unsigned char * hash) const;
	void clear(void);
	virtual bool operator ==(const Trie& other) const;
};

}

#endif /* FSTRIE_H_ */
