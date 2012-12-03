/*
 * KeyValueCountingBloomFilter.h
 *
 *      Author: Till Lorentzen
 */

#ifndef KEYVALUECOUNTINGBLOOMFILTER_H_
#define KEYVALUECOUNTINGBLOOMFILTER_H_
#include <setsync/bloom/CountingBloomFilter.h>
#include <setsync/bloom/FSBloomFilter.h>
#include <setsync/bloom/ComparableBloomFilter.h>
#include <setsync/storage/KeyValueStorage.h>
#include <setsync/DiffHandler.h>

namespace bloom {

class KeyValueCountingBloomFilter: public CountingBloomFilter,
		public FSBloomFilter,
		public ComparableBloomFilterInterface {
	friend class KeyValueCountingBloomFilterTest;
protected:
	setsync::storage::AbstractKeyValueStorage& storage_;
public:
	KeyValueCountingBloomFilter(const utils::CryptoHash& hash,
			setsync::storage::AbstractKeyValueStorage& storage,
			const uint64_t maxNumberOfElements = 10000,
			const bool hardMaximum = false,
			const float falsePositiveRate = 0.001);
	virtual ~KeyValueCountingBloomFilter();
	virtual void diff(const unsigned char * externalBF,
			const std::size_t length, const std::size_t offset,
			setsync::AbstractDiffHandler& handler) const;
	/**
	 *  adds this given key to bloom filter and berkeley db
	 *  \param key to be added
	 */
	void virtual add(const unsigned char * key);
	/**
	 * \param keys to be added to the BloomFilter
	 * \param count number of keys in the array
	 * \throws an Exception, if the maximum is reached and hardMaximum has been set
	 */
	virtual void addAll(const unsigned char *keys, const std::size_t count);
	/*
	 * Removes the given key from the bloom filter if the filter contains the key
	 * \param key the key to be deleted
	 * \return true on success
	 */
	virtual bool remove(const unsigned char * key);
	/**
	 * cleans the berkeley db and the bloom filter
	 */
	virtual void clear(void);
};

}

#endif /* KEYVALUECOUNTINGBLOOMFILTER_H_ */
