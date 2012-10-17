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

class DBBloomFilter: public CountingBloomFilter,
		public FSBloomFilter,
		public berkeley::BerkeleyDBTableUserInferface {
private:
	Db * db_;
public:
	DBBloomFilter(Db * db, const uint64_t maxNumberOfElements = 10000,
			const bool hardMaximum = false,
			const float falsePositiveRate = 0.001,
			const std::size_t hashsize = SHA_DIGEST_LENGTH);
	void virtual add(const unsigned char * key);
	virtual bool remove(const unsigned char * key);
	virtual ~DBBloomFilter();
	static const char * getLogicalDatabaseName(){
		return "bloom";
	}
	static const DBTYPE getTableType(){
		return DB_HASH;
	}
	static const u_int32_t getTableFlags() {
		return DB_DUPSORT;
	}
};

}

#endif /* DBBLOOMFILTER_H_ */
