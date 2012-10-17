/*
 * CountingBloomFilter.cpp
 *
 *      Author: Till Lorentzen
 */

#include "CountingBloomFilter.h"
#include <string.h>
namespace bloom {

bool CountingBloomFilter::remove(const char * string) {
	unsigned char c[SHA_DIGEST_LENGTH];
	SHA1((unsigned char*) string, strlen(string), c);
	return remove(c);
}

bool CountingBloomFilter::remove(const std::string& string) {
	unsigned char c[SHA_DIGEST_LENGTH];
	SHA1((unsigned char*) string.c_str(), string.size(), c);
	return remove(c);
}
}
