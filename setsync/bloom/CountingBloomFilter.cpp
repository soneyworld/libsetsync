/*
 * CountingBloomFilter.cpp
 *
 *      Author: Till Lorentzen
 */

#include "CountingBloomFilter.h"
#include <string.h>
namespace bloom {

CountingBloomFilter::CountingBloomFilter() {
	// TODO Auto-generated constructor stub

}

CountingBloomFilter::CountingBloomFilter(const CountingBloomFilter& filter) {
	// TODO Auto-generated constructor stub

}

CountingBloomFilter::~CountingBloomFilter() {
	// TODO Auto-generated destructor stub
}
bool CountingBloomFilter::remove(const char * key){
	unsigned char c[SHA_DIGEST_LENGTH];
	SHA1((unsigned char*) key, strlen(key), c);
	return remove(c);
}


bool CountingBloomFilter::remove(const std::string& key){
	unsigned char c[SHA_DIGEST_LENGTH];
	SHA1((unsigned char*) key.c_str(), key.size(), c);
	return remove(c);
}
}
