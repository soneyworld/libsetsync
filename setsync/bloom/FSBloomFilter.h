/*
 * FSBloomFilter.h
 *
 *      Author: Till Lorentzen
 */

#ifndef FSBLOOMFILTER_H_
#define FSBLOOMFILTER_H_

#include "BloomFilter.h"

namespace bloom {

class FSBloomFilter: public virtual bloom::AbstractBloomFilter {
	friend class FSBloomFilterTest;
public:
	/**
	 * \param maxNumberOfElements which should be represented by the bloom filter
	 * \param hardMaximum ensures that the the maximum of storable entries will never be exceeded
	 * \param falsePositiveRate can be set to any value ]0,1[.
	 * \param hashsize sets the size of the inserted keys. 20 bytes for SHA1 for example.
	 */
	FSBloomFilter(const utils::CryptoHash& hash, const char * file = NULL,
			const uint64_t maxNumberOfElements = 10000,
			const bool hardMaximum = false,
			const float falsePositiveRate = 0.001);
	virtual ~FSBloomFilter();
	virtual void load(std::istream &in, const uint64_t numberOfElements);
	virtual uint64_t save(std::ostream &out);
	virtual void clear();
	/**
	 * Adds a given hash key to the bloom filter
	 * \param key which should be added
	 * \throws an Exception, if the maximum is reached and hardMaximum has been set
	 */
	virtual void add(const unsigned char *key);
	/**
	 * \param keys to be added to the BloomFilter
	 * \param count number of keys in the array
	 * \throws an Exception, if the maximum is reached and hardMaximum has been set
	 */
	virtual void addAll(const unsigned char *keys, const std::size_t count);
	/**
	 * \param key a simple pointer to the stored key, which should be checked
	 * \return true if the given key seems to have been inserted in past
	 */
	virtual bool contains(const unsigned char *key) const;
	/**
	 * \param keys a simple array of keys, which should be checked, if they are represented by the bloom filter
	 * \param count the length of the keys array
	 * \return the given count on containing all keys, otherwise 0
	 */
	virtual std::size_t containsAll(const unsigned char *keys,
			const std::size_t count) const;
	/**
	 * Checks, if both BloomFilter have got exact the same bit array
	 * \return true if the bloom filter bit array is the same
	 */
	virtual bool operator ==(const AbstractBloomFilter& filter) const;
	/**
	 * Checks, if there is a difference between the bit arrays
	 * \return true if there is minimal 1 bit difference between the bit arrays
	 */
	virtual bool operator !=(const AbstractBloomFilter& filter) const;
	/**
	 * \return the intersection between both bloom filter
	 */
	virtual AbstractBloomFilter& operator &=(const AbstractBloomFilter& filter);
	/**
	 * \return the union of both bloom filter
	 */
	virtual AbstractBloomFilter& operator |=(const AbstractBloomFilter& filter);
	/**
	 * \return the difference between the both bloom filter
	 */
	virtual AbstractBloomFilter& operator ^=(const AbstractBloomFilter& filter);
	/**
	 * \return a string containing the filter as a sequence of zeros and ones
	 */
	virtual std::string toString();
protected:
	/// Help function to get the right positions inside the byte array of the bloom filter
	void compute_indices(const uint64_t hash, std::size_t& bit_index,
			std::size_t& bit) const;
	/// The bloom filter memory mapped storage
	unsigned char * bitArray_;
	FILE * filehandler_;
	unsigned int mmapLength_;
private:
	void init(const float falsePositiveRate, const bool hardMaximum,
			const uint64_t numberOfElements);
};

}

#endif /* FSBLOOMFILTER_H_ */
