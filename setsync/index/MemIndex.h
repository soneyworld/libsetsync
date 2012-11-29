/*
 * MemIndex.h
 *
 *      Author: Till Lorentzen
 */

#ifndef MEMINDEX_H_
#define MEMINDEX_H_

#include "AbstractSetIndex.h"
#include <map>
#include <string.h>

namespace setsync {

namespace index {
/**
 * Class to compare the hashes to where the given pointer are pointing
 *
 */
class HashCompare {
public:
	/**
	 * \param hashSize of the keys, which should be compared
	 */
	HashCompare(const std::size_t hashSize);
	/**
	 * \param key1
	 * \param key2
	 * \return true, if key1 > key2
	 */
	bool
	operator()(const unsigned char * key1, const unsigned char * key2) const;
private:
	/// Size of the hash keys in bytes
	const std::size_t size_;
};

class MemIndex: public setsync::index::AbstractSetIndex {
private:
	/// Map to store the entries
	std::map<unsigned char *, void *, HashCompare> * hashmap;
public:
	MemIndex(const utils::CryptoHash& hash);
	virtual ~MemIndex();
	virtual bool put(const unsigned char * hash, const void * data,
			const std::size_t length);
	virtual void * get(const unsigned char * hash, std::size_t * length) const;
	virtual bool remove(const unsigned char * hash);
	virtual void clear(void);
};

}

}

#endif /* MEMINDEX_H_ */
