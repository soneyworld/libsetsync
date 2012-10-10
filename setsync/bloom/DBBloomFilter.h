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

namespace bloom {

class DBBloomFilter: public CountingBloomFilter , public FSBloomFilter {
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
};

}

#endif /* DBBLOOMFILTER_H_ */
