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
namespace bloom {

/**
 * Basic BlommFilter class
 *
 */
class BloomFilter {
public:
	/**
	 * \param maxNumberOfElements which should be represented by the bloom filter (default value = 10000)
	 * \param hardMaximum ensures that the the maximum of storable entries will never be exceeded
	 * \param falsePositiveRate can be set to any value ]0,1[.
	 */
	BloomFilter(const uint64_t maxNumberOfElements = 10000,
			const bool hardMaximum = false,
			const float falsePositiveRate = 0.001);
	/**
	 * Simple copy constructor
	 */
	BloomFilter(const BloomFilter& filter);
	virtual ~BloomFilter();

	/**
	 * Loads a given array as bloom filter into the internal storage
	 */
	void load(const unsigned char* data, size_t len);

	BloomFilter& operator=(const BloomFilter& filter);

	virtual bool operator!() const;
	/**
	 * Resets the bloom filter
	 */
	virtual void clear();
	/**
	 * Returns the size of the bloom filter in bytes
	 */
	virtual std::size_t size() const;
	/**
	 * Checks, if both BloomFilter have got exact the same bit array
	 */
	virtual bool operator ==(const BloomFilter& filter);
	/**
	 * Checks, if there is a difference between the bit arrays
	 */
	virtual bool operator !=(const BloomFilter& filter);
	/**
	 * Returns the intersection between both bloom filter
	 */
	virtual BloomFilter& operator &=(const BloomFilter& filter);
	/**
	 * Returns the union of both bloom filter
	 */
	virtual BloomFilter& operator |=(const BloomFilter& filter);
	/**
	 * Returns the difference between the both bloom filter
	 */
	virtual BloomFilter& operator ^=(const BloomFilter& filter);
protected:
	/// Exact size of the filter in bits
	std::size_t filterSize_;
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
};

}

#endif /* BLOOMFILTER_H_ */
