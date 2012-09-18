/*
 * BloomFilter.h
 *
 *      Author: Till Lorentzen
 *
 *      The BloomFilter Class is a simple in-memory bloom filter implementation.
 *		All more specific bloom filter should be used like this implementation.
 *
 *		Known subclasses are the CountingBloomFilter.
 */

#ifndef BLOOMFILTER_H_
#define BLOOMFILTER_H_
#include <cstddef>
#include <stdint.h>
#include <iostream>
#include "HashFunction.h"
#include <setsync/sha1.h>
#ifndef BYTESIZE
#define BYTESIZE 8
#endif
///Contains all variations of bloom filter
namespace bloom {

/**
 * Basic BlommFilter class
 *
 */
class BloomFilter {
protected:
	static const unsigned char bit_mask[BYTESIZE];
public:
	/**
	 * \param maxNumberOfElements which should be represented by the bloom filter
	 * \param hardMaximum ensures that the the maximum of storable entries will never be exceeded
	 * \param falsePositiveRate can be set to any value ]0,1[.
	 * \param hashsize sets the size of the inserted keys. 20 bytes for SHA1 for example.
	 */
	BloomFilter(const uint64_t maxNumberOfElements = 10000,
			const bool hardMaximum = false,
			const float falsePositiveRate = 0.001,
			const std::size_t hashsize = SHA_DIGEST_LENGTH);
	/**
	 * \param hashFunction is the Function which should be used by the BloomFilter
	 * \param maxNumberOfElements which should be represented by the bloom filter
	 * \param hardMaximum ensures that the the maximum of storable entries will never be exceeded
	 * \param falsePositiveRate can be set to any value ]0,1[.
	 * \param hashsize sets the size of the inserted keys. 20 bytes for SHA1 for example.
	 */
	BloomFilter(HashFunction* hashFunction,
			const uint64_t maxNumberOfElements = 10000,
			const bool hardMaximum = false,
			const float falsePositiveRate = 0.001,
			const std::size_t hashsize = SHA_DIGEST_LENGTH);
	/**
	 * Simple copy constructor
	 * \param filter the BloomFilter instance, which should be copied
	 */
	BloomFilter(const BloomFilter& filter);
	virtual ~BloomFilter();

	/**
	 * Loads a given array as bloom filter into the internal storage
	 */
	virtual void load(std::istream &in);

	BloomFilter& operator=(const BloomFilter& filter);

	virtual bool operator!() const;
	/**
	 * Resets the bloom filter
	 */
	virtual void clear();
	/**
	 * \return the size of the bloom filter in bytes (rounded up, if last byte is not full)
	 */
	virtual std::size_t size() const;
	/**
	 * \return the size of the bloom filter in bits
	 */
	virtual uint64_t exactBitSize() const;
	/**
	 * Checks, if both BloomFilter have got exact the same bit array
	 * \return true if the bloom filter bit array is the same
	 */
	virtual bool operator ==(const BloomFilter& filter) const;
	/**
	 * Checks, if there is a difference between the bit arrays
	 * \return true if there is minimal 1 bit difference between the bit arrays
	 */
	virtual bool operator !=(const BloomFilter& filter) const;
	/**
	 * \return the intersection between both bloom filter
	 */
	virtual BloomFilter& operator &=(const BloomFilter& filter);
	/**
	 * \return the union of both bloom filter
	 */
	virtual BloomFilter& operator |=(const BloomFilter& filter);
	/**
	 * \return the difference between the both bloom filter
	 */
	virtual BloomFilter& operator ^=(const BloomFilter& filter);
	/**
	 * Adds a given hash key to the bloom filter
	 * \param key which should be added
	 * \throws an Exception, if the maximum is reached and hardMaximum has been set
	 */
	virtual void add(const unsigned char *key);
	/**
	 * \param key will be hashed and the hash will be added to bloom filter
	 * \throws an Exception, if the maximum is reached and hardMaximum has been set
	 */
	void add(const std::string& key);
	/**
	 * \param key a simple pointer to the stored key, which should be checked
	 * \return true if the given key seems to have been inserted in past
	 */
	virtual bool contains(const unsigned char *key) const;
	/**
	 * \param key will be hashed and the hash will be checked on the bloom filter
	 */
	bool contains(const std::string& key) const;
	/**
	 * \param keys a simple array of keys, which should be checked, if they are represented by the bloom filter
	 * \param count the length of the keys array
	 * \return the given count on containing all keys, otherwise the first failed position in the given key array
	 */
	virtual std::size_t containsAll(const unsigned char *keys,
			const std::size_t count) const;

	//	virtual bool isEmpty();
protected:
	/// Exact size of the filter in bits
	uint64_t filterSize_;
	/// The bloom filter memory storage
	unsigned char * bitArray_;
	/// A simple counter of the inserted elements
	uint64_t itemCount_;
	/// Number of hash functions to be used
	unsigned int functionCount_;
	/**
	 *  If this is set, only the number of given elements in the constructor
	 *  could be inserted, otherwise, more elements could be saved.
	 *  If more elements are inserted, the false positive rate cannot be guaranteed.
	 */
	bool hardMaximum_;
	/// The hard limit of elements, if hardMaximum is set
	uint64_t maxElements_;
	/// Size of the given hash keys
	std::size_t hashsize_;
	HashFunction* hashFunction_;
	void compute_indices(const uint64_t hash, std::size_t& bit_index,
			std::size_t& bit) const;
private:
	void init(const float falsePositiveRate, const bool hardMaximum,
			const uint64_t numberOfElements);
};

}

#endif /* BLOOMFILTER_H_ */
