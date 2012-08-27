/*
 * SQLiteTrie.h
 *
 *  Created on: 27.08.2012
 *      Author: Till Lorentzen
 */

#ifndef SQLITETRIE_H_
#define SQLITETRIE_H_
#include "Trie.h"


namespace trie{
class SQLiteTrie: public Trie {
public:
	SQLiteTrie();
	virtual ~SQLiteTrie();
};

}
#endif /* SQLITETRIE_H_ */
