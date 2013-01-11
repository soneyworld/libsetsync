/*
 * BDBTransactionsTest.cpp
 *
 *      Author: Till Lorentzen
 */

#include "BDBTransactionsTest.h"
#include <setsync/utils/FileSystem.h>
#include <setsync/utils/BerkeleyDB.h>
#include <sys/stat.h>
#include "StopWatch.h"

using namespace std;

BDBTransactionsTest::BDBTransactionsTest(const DBTYPE type) {
	int ret;
	setsync::utils::FileSystem::rmDirRecursive("temp-env");
	mkdir("temp-env", 0700);
	this->env_ = new DbEnv(0);
	u_int32_t gb = 0;
	u_int32_t bytes = 2000*1024;
	this->env_->set_cachesize(gb,bytes,1);
	ret = this->env_->open("temp-env", DB_INIT_MPOOL | DB_CREATE | DB_INIT_TXN,
			0);
	if (ret != 0)
		throw DbException(ret);
	this->db_ = new Db(this->env_, 0);
	ret = this->db_->open(NULL, "test", "test", type,
			DB_CREATE | DB_AUTO_COMMIT, 0);
	if (ret != 0)
		throw DbException(ret);
}

BDBTransactionsTest::~BDBTransactionsTest() {
	this->db_->close(0);
	this->env_->close(0);
	delete this->db_;
	delete this->env_;
	setsync::utils::FileSystem::rmDirRecursive("temp-env");
}

void BDBTransactionsTest::run() {
	cout << "running Berkeley DB transaction test ("
			<< setsync::utils::BerkeleyDB::tableTypeToString(db_) << ")" << endl;
	cout << "notrans,CPUintervalDuration,realtimeIntervalDuration,duration"
			<< endl;
	Dbt value(NULL, 0);
	unsigned int count = 0;
	StopWatch stopwatch;
	double duration = 0;
	for (uint64_t i = 0; i < ITERATIONS; i++) {
		stopwatch.start();
		DbTxn * txn;
		this->env_->txn_begin(NULL, &txn, 0);
		Dbt key(&i, sizeof(uint64_t));
		this->db_->put(txn, &key, &value, 0);
		txn->commit(0);
		stopwatch.stop();
		if (count == ITEMS_PER_LOOPS) {
			duration += stopwatch.getDuration();
			cout << i << "," << stopwatch.getCPUDuration() << ","
					<< stopwatch.getDuration() << "," << duration << endl;
			count = 0;
			stopwatch.reset();
		}
		count++;
	}
	duration += stopwatch.getDuration();
	cout << ITERATIONS << "," << stopwatch.getCPUDuration() << ","
			<< stopwatch.getDuration() << "," << duration << endl;
}
