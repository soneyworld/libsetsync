/*
 * SQLiteBloomFilter.h
 *
 *      Author: Till Lorentzen
 */

#ifndef SQLITEBLOOMFILTER_H_
#define SQLITEBLOOMFILTER_H_

#include "BloomFilter.h"
#include "setsync/index/SQLiteDatabase.h"
#include "setsync/index/SQLiteIndex.h"
#include <sqlite3.h>

namespace bloom {

class SQLiteBloomFilter: public bloom::BloomFilter {
private:
	sqlite::SQLiteDatabase * db;
	sqlite::SQLiteIndex * index;
	bool tempDatabase;
public:
	SQLiteBloomFilter(sqlite::SQLiteDatabase * db,
			const std::string hashFunction,
			const uint64_t maxNumberOfElements = 10000,
			const bool hardMaximum = false,
			const float falsePositiveRate = 0.001,
			const std::size_t hashsize = SHA_DIGEST_LENGTH);
	SQLiteBloomFilter(const std::string hashFunction,
			const uint64_t maxNumberOfElements = 10000,
			const bool hardMaximum = false,
			const float falsePositiveRate = 0.001,
			const std::size_t hashsize = SHA_DIGEST_LENGTH);
	SQLiteBloomFilter(const uint64_t maxNumberOfElements = 10000,
			const bool hardMaximum = false,
			const float falsePositiveRate = 0.001,
			const std::size_t hashsize = SHA_DIGEST_LENGTH);
	void add(const unsigned char *key);
	virtual ~SQLiteBloomFilter();
};

}

#endif /* SQLITEBLOOMFILTER_H_ */
