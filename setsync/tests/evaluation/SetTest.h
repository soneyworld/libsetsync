/*
 * SetTest.h
 *
 *      Author: Till Lorentzen
 */

#ifndef SETTEST_H_
#define SETTEST_H_

#include "Test.h"
#include <setsync/storage/KeyValueStorage.h>
#include <setsync/Set.hpp>
#include <setsync/bloom/KeyValueCountingBloomFilter.h>
#include <setsync/trie/KeyValueTrie.h>
#include <setsync/index/KeyValueIndex.h>
#include <setsync/utils/CryptoHash.h>

namespace evaluation {

enum StorageType {
	LEVELDB, BERKELEYDB
};

class SetTest: public Test {
private:
	setsync::storage::AbstractKeyValueStorage * bfstore;
	setsync::storage::AbstractKeyValueStorage * triestore;
	setsync::storage::AbstractKeyValueStorage * indexstore;
public:
	SetTest(StorageType type);
	virtual ~SetTest();
	virtual void run();
};

}

#endif /* SETTEST_H_ */
