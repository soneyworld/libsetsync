/*
 * CountingBloomFilter.h
 *
 *      Author: Till Lorentzen
 */

#ifndef COUNTINGBLOOMFILTER_H_
#define COUNTINGBLOOMFILTER_H_
#include "BloomFilter.h"

namespace bloom {
/**
 * A counting bloom filter extends an abstract bloom filter by adding
 * a remove functionality. It is also an abstract class, but it adds
 * helping functions for calling the remove method with strings.
 */
class CountingBloomFilter: public virtual AbstractBloomFilter {
public:
	/**
	 * Remove the given crypto key from the BloomFilter
	 * \param key to be removed
	 * \return true on successful remove
	 */
	virtual bool remove(const unsigned char * key) = 0;
	/**
	 * Hash the given string and remove the hash from the BloomFilter
	 * \param string to be hashed and removed
	 * \return true on successful remove
	 */
	virtual bool remove(const char * string);
	/**
	 * Hash the given string and remove the hash from the BloomFilter
	 * \param string to be hashed and removed
	 * \return true on successful remove
	 */
	virtual bool remove(const std::string& string);

	CountingBloomFilter(const utils::CryptoHash& hash);
	virtual ~CountingBloomFilter();
};

}

#endif /* COUNTINGBLOOMFILTER_H_ */
