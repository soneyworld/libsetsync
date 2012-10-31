/*
 * Trie.h
 *
 *      Author: Till Lorentzen
 */

#ifndef TRIE_H_
#define TRIE_H_

#include <cstring>
#include <string>
#include <setsync/utils/CryptoHash.h>

namespace trie {

class Trie {
private:
	size_t size;
	bool needsHashPerforming;
protected:
	const utils::CryptoHash& hash_;
	void setSize(const size_t size);
	void incSize();
	void decSize();
	bool isHashPerformingNedded();
	void setHashingPerformed();
public:
	Trie(const utils::CryptoHash& hash);
	size_t getSize() const;
	const utils::CryptoHash& getHash() const;
	unsigned char * hashscratch;
	virtual bool add(const char * str);
	virtual bool add(const char * str, bool performhash);
	virtual bool add(const std::string& str);
	virtual bool add(const std::string& str, bool performhash);
	virtual bool add(const unsigned char * hash);
	virtual bool add(const unsigned char * hash, bool performhash) = 0;
	virtual bool remove(const char * str);
	virtual bool remove(const char * str, bool performhash);
	virtual bool remove(const std::string& str);
	virtual bool remove(const std::string& str, bool performhash);
	virtual bool remove(const unsigned char * hash);
	virtual bool remove(const unsigned char * hash, bool performhash) = 0;
	virtual bool contains(const char * str) const;
	virtual bool contains(const std::string& str) const;
	virtual bool contains(const unsigned char * hash) const = 0;
	virtual void clear(void) = 0;
	virtual ~Trie();
	/**
	 * Checks, if both Trie have got exact the same hash key as root
	 * \return true if the trie instances have got same root
	 */
	virtual bool operator ==(const Trie& other) const = 0;
	/**
	 * Checks, if both Trie have got a different hash key as root
	 * \return true if the trie instances have got different root
	 */
	virtual bool operator !=(const Trie& other) const {
		return ! this->operator ==(other);
	}
};

}

#endif /* TRIE_H_ */
