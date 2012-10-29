/*
 * DBBloomFilter.h
 *
 *      Author: Till Lorentzen
 */

#ifndef DBBLOOMFILTER_H_
#define DBBLOOMFILTER_H_
#include <db_cxx.h>
#include "CountingBloomFilter.h"
#include "FSBloomFilter.h"
#include <setsync/BerkeleyDBTableUserInterface.h>
#include <setsync/DiffHandler.h>

namespace bloom {

class DBBloomFilter;
/**
 * An object which is used to save and load the bloom filter settings to and
 * from the berkeley DB.
 */
class DbBloomFilterSetting {
	friend class DBBloomFilter;
private:
	/**
	 * Constructs an object and sets the member variables
	 */
	DbBloomFilterSetting(const uint64_t maxNumberOfElements,
			const bool hardMaximum, const float falsePositiveRate,
			const std::size_t hashsize);
	/**
	 * Loads the member variables from the given array.
	 *
	 * \param toLoad pointer to the loaded array
	 */
	DbBloomFilterSetting(void * toLoad);
	/**
	 * Returns the size of the marshalled array containing all member variables in bytes
	 *
	 * \return buffer size for marshalling in bytes
	 */
	static const std::size_t getBufferSize();
	/**
	 * Copies the given reference values into the given target array.
	 * The given target array must be allocated before this is called.
	 * The needed size of the target array can be requested by calling
	 * DbBloomFilterSetting::getBufferSize();
	 *
	 * \param target pointer to the target array, in which the reference values should be copied
	 * \param toBeMarshalled reference to the setting object which should be marshalled
	 */
	static void marshall(void * target,
			const DbBloomFilterSetting& toBeMarshalled);
public:
	/// maximum number of elements, which should be saved
	uint64_t maxNumberOfElements;
	/// true if maxNumberOfElements is a hard limit
	bool hardMaximum;
	/// false positive ratio of the bloom filter
	float falsePositiveRate;
	/// Size of the crypto hash key in bytes
	std::size_t hashSize;
};
/**
 * Berkeley DB based bloom filter. It saves all inserted hashes
 * and its position in the filter into a given Berkeley DB.
 *
 * The added crypto keys will be added to db as vales by calculating the position
 * in the FSBloomFilter as key and the the crypto key as value.
 * So it could happen, that more than one crypto hash is saved for one position
 * in the bloom filter. That is possible by saving multiple values per key.
 */
class DBBloomFilter: public CountingBloomFilter,
		public FSBloomFilter,
		public berkeley::BerkeleyDBTableUserInferface {
private:
	/// Given Berkeley DB pointer
	Db * db_;
	/// The unique key name of the settings record in the DB
	static const char setting_name[];
public:
	/**
	 * To create a new DBBloomFilter, create a new Berkeley db with the
	 * flags, statically given by DBBloomFilter::getTableFlags. Then open
	 * the db with the statically available DBBloomFilter::getTableType and
	 * the table name DBBloomFilter::getLogicalDatabaseName. Then give the
	 * pointer to the db to this constructor. The Berkeley DB will not be
	 * closed on destroying this instance. It has to be closed manually after
	 * destroying.
	 *
	 * \param db pointer to opened berkeley db
	 * \param maxNumberOfElements will be passed to FSBloomFilter
	 * \param hardMaximum will be passed to FSBloomFilter
	 * \param falsePositiveRate will be passed to FSBloomFilter
	 * \param hashsize will be passed to FSBloomFilter
	 */
	DBBloomFilter(Db * db, const uint64_t maxNumberOfElements = 10000,
			const bool hardMaximum = false,
			const float falsePositiveRate = 0.001,
			const std::size_t hashsize = SHA_DIGEST_LENGTH);
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

	void diff(const unsigned char * externalBF, const std::size_t length,
				const std::size_t offset, setsync::DiffHandler& handler);
	/**
	 * Default destructor
	 */
	virtual ~DBBloomFilter();
	/**
	 *  \return the name of the database table: "bloom"
	 */
	static const char * getLogicalDatabaseName() {
		return "bloom";
	}
	/**
	 * \return the type of the database table: DB_HASH
	 */
	static const DBTYPE getTableType() {
		return DB_HASH;
	}
	/**
	 * \return the needed flags to be set, before opening berkeley db
	 */
	static const u_int32_t getTableFlags() {
		return DB_DUPSORT;
	}
	/**
	 * If there is any bloom filter settings saved on the DB, this method
	 * will return the values. If none is available, an exception is thrown
	 * \param db pointer to the DB, in which the settings are saved
	 * \return loaded settings
	 */
	static const DbBloomFilterSetting loadSettings(Db * db);
	/**
	 * Saves the given settings to the given DB
	 * \param db pointer to the DB
	 * \param maxNumberOfElements of the bloom filter
	 * \param hardMaximum true, if the bloom filter do not accept more than maxNumberOfElements
	 * \param falsePositiveRate of the bloom filter
	 * \param hashsize size of the given crypto keys in bytes
	 */
	static void saveSettings(Db * db, const uint64_t maxNumberOfElements,
			const bool hardMaximum, const float falsePositiveRate,
			const std::size_t hashsize);
};

}

#endif /* DBBLOOMFILTER_H_ */
