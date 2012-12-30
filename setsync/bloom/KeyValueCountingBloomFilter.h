/*
 * KeyValueCountingBloomFilter.h
 *
 *      Author: Till Lorentzen
 */

#ifndef KEYVALUECOUNTINGBLOOMFILTER_H_
#define KEYVALUECOUNTINGBLOOMFILTER_H_
#include <setsync/sync/Synchronization.h>
#include <setsync/bloom/CountingBloomFilter.h>
#include <setsync/bloom/FSBloomFilter.h>
#include <setsync/bloom/ComparableBloomFilter.h>
#include <setsync/storage/KeyValueStorage.h>
#include <setsync/DiffHandler.h>
#include <setsync/Packets.h>

namespace bloom {
class KeyValueCountingBloomFilter;

class KeyValueBloomFilterSync: public setsync::sync::AbstractSyncProcessPart {
private:
	KeyValueCountingBloomFilter * bf_;
	std::size_t inPos_;
	std::size_t outPos_;
public:
	KeyValueBloomFilterSync(KeyValueCountingBloomFilter * bf);
	virtual ~KeyValueBloomFilterSync();
	/**
	 * \return true, if more output is available
	 */
	virtual bool pendingOutput() const;
	/**
	 * \return true, if the sync process expects  more input
	 */
	virtual bool awaitingInput() const;

	virtual std::size_t processInput(void * inbuf, const std::size_t length,
			setsync::AbstractDiffHandler& diffhandler);
	virtual std::size_t writeOutput(void * outbuf, const std::size_t maxlength);
	virtual std::size_t getRemainigOutputPacketSize() const;
};

/**
 * This BloomFilter is an implementation of a counting bloomfilter,
 * which uses the given Key-Value store to load and safe the hashes
 * added to the Filter. The filter itself remains in a memory mapped
 * file.
 */
class KeyValueCountingBloomFilter: public CountingBloomFilter,
		public FSBloomFilter,
		public ComparableBloomFilterInterface,
		public ::setsync::sync::SyncableDataStructureInterface {
	friend class KeyValueCountingBloomFilterTest;
	friend class KeyValueBloomFilterSyncTest;
	friend class KeyValueBloomFilterSync;
protected:
	setsync::storage::AbstractKeyValueStorage& storage_;
public:
	/**
	 * To create a new KeyValueCountingBloomFilter, create a new
	 * AbstractKeyValueStorage and pass it as second parameter.
	 * The storage will be used to save the bit position in the
	 * bloom filter as key and the corresponding hashes as value.
	 * If more then one cryptographic hash corresponds to one position,
	 * all hashes are saved as an array
	 *
	 * \param hash the crypto hash function, used for keys
	 * \param storage to load and save the bloom filter entries
	 * \param maxNumberOfElements will be passed to FSBloomFilter
	 * \param hardMaximum will be passed to FSBloomFilter
	 * \param falsePositiveRate will be passed to FSBloomFilter
	 */
	KeyValueCountingBloomFilter(const utils::CryptoHash& hash,
			setsync::storage::AbstractKeyValueStorage& storage,
			const std::string& file,
			const uint64_t maxNumberOfElements = 10000,
			const bool hardMaximum = false,
			const float falsePositiveRate = 0.001);
	virtual ~KeyValueCountingBloomFilter();
	/**
	 * Calculates the difference between the given part of the
	 * external bloom filter and the local one. Each hash, which
	 * is locally available, but is not set on the external bloom
	 * filter, is passed to the given handler. The offset is given
	 * to locate the local position of the given fragment of the
	 * external bloom filter
	 *
	 * \param externalBF fragment
	 * \param length of the bloom filter fragment
	 * \param offset from the beginning of the bloom filter
	 * \param handler to be informed about different hashes
	 */
	virtual void diff(const unsigned char * externalBF,
			const std::size_t length, const std::size_t offset,
			setsync::AbstractDiffHandler& handler) const;
	/**
	 *  adds this given key to bloom filter and the key value storage
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
	 * cleans the key value storage and the bloom filter
	 */
	virtual void clear(void);

	virtual setsync::sync::AbstractSyncProcessPart * createSyncProcess();
};

}

#endif /* KEYVALUECOUNTINGBLOOMFILTER_H_ */
