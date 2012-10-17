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

namespace bloom {
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
};

}

#endif /* DBBLOOMFILTER_H_ */
