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
#include <setsync/utils/CryptoHash.h>
#ifndef BYTESIZE
#define BYTESIZE 8
#endif
///Contains all variations of bloom filter
namespace bloom {

class AbstractBloomFilter {
public:
	/**
	 * Loads a given input stream as bloom filter into the internal storage
	 * \param in source stream
	 * \param numberOfElements The count of Elements, which has been save in the stream
	 */
	virtual void load(std::istream &in, const uint64_t numberOfElements) = 0;
	/**
	 * Writes the bloom filter into the given out stream
	 * \param out the target stream
	 */
	virtual uint64_t save(std::ostream &out) = 0;
	/**
	 * Resets the bloom filter
	 */
	virtual void clear() = 0;
	/**
	 * \return the size of the bloom filter in bytes (rounded up, if last byte is not full)
	 */
	virtual std::size_t size() const;
	/**
	 * \return the size of the bloom filter in bits
	 */
	virtual uint64_t exactBitSize() const;
	/**
	 * \return the number of saved items
	 */
	virtual uint64_t numberOfElements() const;
	/**
	 * \param string will be hashed and the hash will be added to bloom filter
	 * \throws an Exception, if the maximum is reached and hardMaximum has been set
	 */
	virtual void add(const char *string);
	/**
	 * Adds a given hash key to the bloom filter
	 * \param key which should be added
	 * \throws an Exception, if the maximum is reached and hardMaximum has been set
	 */
	virtual void add(const unsigned char *key) = 0;
	/**
	 * \param key will be hashed and the hash will be added to bloom filter
	 * \throws an Exception, if the maximum is reached and hardMaximum has been set
	 */
	virtual void add(const std::string& key);

	/**
	 * \param keys to be added to the BloomFilter
	 * \param count number of keys in the array
	 * \throws an Exception, if the maximum is reached and hardMaximum has been set
	 */
	virtual void addAll(const unsigned char *keys, const std::size_t count) = 0;
	/**
	 * \param string will be hashed and the hash will be checked on the bloom filter
	 */
	virtual bool contains(const std::string& string) const;
	/**
	 * \param string will be hashed and the hash will be checked on the bloom filter
	 */
	virtual bool contains(const char * string) const;

	/**
	 * \param key a simple pointer to the stored key, which should be checked
	 * \return true if the given key seems to have been inserted in past
	 */
	virtual bool contains(const unsigned char *key) const = 0;
	/**
	 * \param keys a simple array of keys, which should be checked, if they are represented by the bloom filter
	 * \param count the length of the keys array
	 * \return the given count on containing all keys, otherwise less then count
	 */
	virtual std::size_t containsAll(const unsigned char *keys,
			const std::size_t count) const = 0;
	/**
	 * Checks, if both BloomFilter have got exact the same bit array
	 * \return true if the bloom filter bit array is the same
	 */
	virtual bool operator ==(const AbstractBloomFilter& filter) const = 0;
	/**
	 * Checks, if there is a difference between the bit arrays
	 * \return true if there is minimal 1 bit difference between the bit arrays
	 */
	virtual bool operator !=(const AbstractBloomFilter& filter) const = 0;
	/**
	 * \return the intersection between both bloom filter
	 */
	virtual AbstractBloomFilter
	& operator &=(const AbstractBloomFilter& filter) = 0;
	/**
	 * \return the union of both bloom filter
	 */
	virtual AbstractBloomFilter
	& operator |=(const AbstractBloomFilter& filter) = 0;
	/**
	 * \return the difference between the both bloom filter
	 */
	virtual AbstractBloomFilter
	& operator ^=(const AbstractBloomFilter& filter) = 0;

protected:
	static const unsigned char bit_mask[BYTESIZE];
	/// Exact size of the filter in bits
	uint64_t filterSize_;
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
	/// Pointer to the used hash Function of the filter
	HashFunction* hashFunction_;
	/// Reference to the crypto hash function used to create the inserted keys
	const utils::CryptoHash& cryptoHashFunction_;
public:
	AbstractBloomFilter(const utils::CryptoHash& hash);
};

/**
 * Basic BlommFilter class
 *
 */
class BloomFilter: public virtual AbstractBloomFilter {
public:
	/**
	 * \param maxNumberOfElements which should be represented by the bloom filter
	 * \param hardMaximum ensures that the the maximum of storable entries will never be exceeded
	 * \param falsePositiveRate can be set to any value ]0,1[.
	 */
	BloomFilter(const utils::CryptoHash& hash,
			const uint64_t maxNumberOfElements = 10000,
			const bool hardMaximum = false,
			const float falsePositiveRate = 0.001);
	/**
	 * Simple copy constructor
	 * \param filter the BloomFilter instance, which should be copied
	 */
	BloomFilter(const BloomFilter& filter);
	virtual ~BloomFilter();

	/**
	 * Loads a given bitarray from an input stream as bloom filter into the internal storage
	 * \param in source stream
	 * \param numberOfElements The count of Elements, which has been save in the stream
	 */
	virtual void load(std::istream &in, const uint64_t numberOfElements);
	/**
	 * Writes the bloom filter bitarray into the given out stream
	 * \param out the target stream
	 */
	virtual uint64_t save(std::ostream &out);

	BloomFilter& operator=(const BloomFilter& filter);

	virtual bool operator!() const;
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
	 * Resets the bloom filter
	 */
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
	 * \return the given count on containing all keys, otherwise the first failed position in the given key array
	 */
	virtual std::size_t containsAll(const unsigned char *keys,
			const std::size_t count) const;
	/**
	 * \return a string containing the filter as a sequence of zeros and ones
	 */
	virtual std::string toString();
protected:
	/// The bloom filter memory storage
	unsigned char * bitArray_;
	/// Help function to get the right positions inside the byte array of the bloom filter
	void compute_indices(const uint64_t hash, std::size_t& bit_index,
			std::size_t& bit) const;
private:
	void init(const float falsePositiveRate, const bool hardMaximum,
			const uint64_t numberOfElements);
};

}

#endif /* BLOOMFILTER_H_ */
