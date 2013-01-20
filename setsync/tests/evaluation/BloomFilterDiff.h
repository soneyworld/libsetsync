/*
 * BloomFilterDiff.h
 *
 *  Created on: 20.01.2013
 *      Author: till
 */

#ifndef BLOOMFILTERDIFF_H_
#define BLOOMFILTERDIFF_H_
#include "Test.h"
#include <setsync/bloom/KeyValueCountingBloomFilter.h>
#include <setsync/crypto/CryptoHash.h>
#include <setsync/storage/MemStorage.h>
using namespace std;
namespace evaluation {

class BloomFilterDiff {
private:
	setsync::storage::MemStorage storageA_;
	setsync::storage::MemStorage storageB_;
	setsync::bloom::KeyValueCountingBloomFilter A_;
	setsync::bloom::KeyValueCountingBloomFilter B_;
	size_t diffsize_;
	float p_;
	size_t salt_;
public:
	BloomFilterDiff(const setsync::crypto::CryptoHash& hash, const size_t A, const float p, const size_t salt);
	virtual ~BloomFilterDiff();
	void run();
};

}

#endif /* BLOOMFILTERDIFF_H_ */
