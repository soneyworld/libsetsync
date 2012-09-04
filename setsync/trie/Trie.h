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
	size_t hashsize;
	bool needsHashPerforming;
protected:
	void setHashSize(size_t size);
	void setSize(size_t size);
	void incSize();
	void decSize();
	bool isHashPerformingNedded();
	void setHashingPerformed();
public:
	Trie();
	size_t getSize();
	virtual bool insert(const unsigned char * hash) = 0;
	virtual bool insert(const unsigned char * hash, bool performhash) = 0;
	virtual bool erase(const unsigned char * hash) = 0;
	virtual bool erase(const unsigned char * hash, bool performhash) = 0;
	virtual void clear(void) = 0;
	virtual ~Trie();
};

}

#endif /* TRIE_H_ */
