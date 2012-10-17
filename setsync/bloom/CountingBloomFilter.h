/*
 * CountingBloomFilter.h
 *
 *      Author: Till Lorentzen
 */

#ifndef COUNTINGBLOOMFILTER_H_
#define COUNTINGBLOOMFILTER_H_
#include "BloomFilter.h"

namespace bloom {

class CountingBloomFilter: public virtual AbstractBloomFilter {
public:
	CountingBloomFilter();
	CountingBloomFilter(const CountingBloomFilter& filter);
	virtual ~CountingBloomFilter();
	virtual bool remove(const unsigned char * key) = 0;
	virtual bool remove(const char * key);
	virtual bool remove(const std::string& key);
};

}

#endif /* COUNTINGBLOOMFILTER_H_ */
