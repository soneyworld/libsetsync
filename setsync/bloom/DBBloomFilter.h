/*
 * DBBloomFilter.h
 *
 *      Author: Till Lorentzen
 */

#ifndef DBBLOOMFILTER_H_
#define DBBLOOMFILTER_H_
#include <db_cxx.h>
#include "CountingBloomFilter.h"

namespace bloom {

class DBBloomFilter: public bloom::CountingBloomFilter {
public:
	DBBloomFilter();
	virtual ~DBBloomFilter();
};

}

#endif /* DBBLOOMFILTER_H_ */
