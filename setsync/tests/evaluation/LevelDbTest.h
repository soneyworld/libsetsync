/*
 * LevelDbTest.h
 *
 *      Author: Till Lorentzen
 */

#ifndef LEVELDBTEST_H_
#define LEVELDBTEST_H_

#include "Test.h"
#include <leveldb/db.h>
#include <setsync/utils/CryptoHash.h>

class LevelDbTest: public Test {
private:
	leveldb::DB* db;
	const utils::CryptoHash& hash;
public:
	LevelDbTest(const utils::CryptoHash& hash);
	virtual ~LevelDbTest();
	virtual void runInsertTest();
	virtual void runRequestTest();
	virtual void runDeletionTest();
	virtual void runInsertDeletionTest();
	virtual void run();
};

#endif /* LEVELDBTEST_H_ */
