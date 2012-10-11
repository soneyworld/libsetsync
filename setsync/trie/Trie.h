/*
 * Trie.h
 *
 *      Author: Till Lorentzen
 */

#ifndef TRIE_H_
#define TRIE_H_

#include <cstring>
#include <setsync/sha1.h>
#include <string>

namespace trie {

class Trie {
private:
	size_t size;
	size_t hashsize_;
	bool needsHashPerforming;
protected:
	void setHashSize(const size_t size);
	void setSize(const size_t size);
	void incSize();
	void decSize();
	bool isHashPerformingNedded();
	void setHashingPerformed();
public:
	Trie(const size_t hashsize = SHA_DIGEST_LENGTH);
	size_t getSize() const;
	size_t getHashSize() const;
	unsigned char * hashscratch;
	virtual bool add(const char * str);
	virtual bool add(const std::string& str);
	virtual bool add(const unsigned char * hash);
	virtual bool add(const unsigned char * hash, bool performhash) = 0;
	virtual bool remove(const char * str);
	virtual bool remove(const std::string& str);
	virtual bool remove(const unsigned char * hash);
	virtual bool remove(const unsigned char * hash, bool performhash) = 0;
	virtual bool contains(const char * str) const;
	virtual bool contains(const std::string& str) const;
	virtual bool contains(const unsigned char * hash) const = 0;
	virtual void clear(void) = 0;
	virtual ~Trie();
};

}

#endif /* TRIE_H_ */
