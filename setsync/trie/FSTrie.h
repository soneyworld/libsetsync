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
};

}

#endif /* FSTRIE_H_ */
