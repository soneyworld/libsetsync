/*
 * FSBloomFilter.h
 *
 *      Author: Till Lorentzen
 */

#ifndef FSBLOOMFILTER_H_
#define FSBLOOMFILTER_H_

#include "BloomFilter.h"

namespace bloom {

class FSBloomFilter: public bloom::AbstractBloomFilter {
public:
	/**
	 * \param maxNumberOfElements which should be represented by the bloom filter
	 * \param hardMaximum ensures that the the maximum of storable entries will never be exceeded
	 * \param falsePositiveRate can be set to any value ]0,1[.
	 * \param hashsize sets the size of the inserted keys. 20 bytes for SHA1 for example.
	 */
	FSBloomFilter(const uint64_t maxNumberOfElements = 10000,
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
	FSBloomFilter(const std::string hashFunction,
			const uint64_t maxNumberOfElements = 10000,
			const bool hardMaximum = false,
			const float falsePositiveRate = 0.001,
			const std::size_t hashsize = SHA_DIGEST_LENGTH);
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
	 * \param key a simple pointer to the stored key, which should be checked
	 * \return true if the given key seems to have been inserted in past
	 */
	virtual bool contains(const unsigned char *key) const;
private:
	/// Help function to get the right positions inside the byte array of the bloom filter
	void compute_indices(const uint64_t hash, std::size_t& bit_index,
			std::size_t& bit) const;
	/// The bloom filter memory mapped storage
	unsigned char * bitArray_;
};

}

#endif /* FSBLOOMFILTER_H_ */
