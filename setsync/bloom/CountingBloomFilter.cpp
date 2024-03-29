/*
 * CountingBloomFilter.cpp
 *
 *      Author: Till Lorentzen
 */

#include "CountingBloomFilter.h"
#include <string.h>
namespace setsync {
namespace bloom {

bool CountingBloomFilter::remove(const char * string) {
	unsigned char c[this->cryptoHashFunction_.getHashSize()];
	this->cryptoHashFunction_(c, string);
	return remove(c);
}

bool CountingBloomFilter::remove(const std::string& string) {
	unsigned char c[this->cryptoHashFunction_.getHashSize()];
	this->cryptoHashFunction_(c, string);
	return remove(c);
}

CountingBloomFilter::CountingBloomFilter(const crypto::CryptoHash& hash) :
	AbstractBloomFilter(hash) {
}
CountingBloomFilter::~CountingBloomFilter() {
}
}
}
