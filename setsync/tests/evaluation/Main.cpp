/*
 * Main.cpp
 *
 *      Author: Till Lorentzen
 */
#include "config.h"
#include "BFTest.h"
#include "DBTest.h"
#include "BDBInsertRemoveTest.h"
#include "BDBTransactionsTest.h"
#include "SetTest.h"
#include "SetSync.h"
#include "StopWatch.h"
#include "SpeedTest.h"
#include <math.h>
#ifdef HAVE_LEVELDB
#include "LevelDbTest.h"
#endif
#include <string>
#include <set>
#include <setsync/Set.hpp>
#include <setsync/config/Configuration.h>
#ifdef HAVE_DB_CXX_H
#include <setsync/storage/MemStorage.h>
#include <setsync/storage/BdbStorage.h>
#endif
#ifdef HAVE_LEVELDB
#include <setsync/storage/LevelDbStorage.h>
#endif
#ifdef HAVE_SQLITE
#include "SQLiteTest.h"
#endif

int main(int ac, char **av) {
	std::set<std::string> args;
	std::set<std::string>::iterator iter;
	for (int i = 1; i < ac; i++) {
		std::string arg(av[i]);
		args.insert(arg);
	}
	if (!(args.find(std::string("--help")) == args.end())) {
		std::cout << "Possible parameter tests are: " << std::endl;
		std::cout << "--bf-test" << std::endl;
		std::cout << "--bdb-test" << std::endl;
		std::cout << "--bdb-insert-remove-test" << std::endl;
		std::cout << "--bdb-transaction-test" << std::endl;
		std::cout << "--set-tests" << std::endl;
#ifdef HAVE_SQLITE
		std::cout << "--sql-test" << std::endl;
#endif
		std::cout << "--insert-get-del-test" << std::endl;
#ifdef HAVE_LEVELDB
		std::cout << "--leveldb-test" << std::endl;
		std::cout << "--leveldb-size-test" << std::endl;
		std::cout << "--leveldb-size-8-0-test" << std::endl;
		std::cout << "--leveldb-size-16-0-test" << std::endl;
		std::cout << "--leveldb-size-20-0-test" << std::endl;
		std::cout << "--leveldb-size-20-100-test" << std::endl;
		std::cout << "--leveldb-size-20-200-test" << std::endl;
#endif
		std::cout << "--all-tests" << std::endl;
		return 0;
	}
	bool speedtest = !(args.find(std::string("--insert-get-del-test"))
			== args.end());
	bool bdb = !(args.find(std::string("--bdb-test")) == args.end());
	bool bf = !(args.find(std::string("--bf-test")) == args.end());
	bool bdbinsert = !(args.find(std::string("--bdb-insert-remove-test"))
			== args.end());
	bool bdbtransaction = !(args.find(std::string("--bdb-transaction-test"))
			== args.end());
	bool all = !(args.find(std::string("--all-tests")) == args.end());
	bool settest = !(args.find(std::string("--set-tests")) == args.end());
#ifdef HAVE_SQLITE
	bool sqlite = !(args.find(std::string("--sql-test")) == args.end());
#endif
#ifdef HAVE_LEVELDB
	bool level = !(args.find(std::string("--leveldb-test")) == args.end());
	bool levelsize = !(args.find(std::string("--leveldb-size-test"))
			== args.end());
	bool levelsize8 = !(args.find(std::string("--leveldb-size-8-0-test"))
			== args.end());
	bool levelsize16 = !(args.find(std::string("--leveldb-size-16-0-test"))
			== args.end());
	bool levelsize20 = !(args.find(std::string("--leveldb-size-20-0-test"))
			== args.end());
	bool levelsize20100 =
			!(args.find(std::string("--leveldb-size-20-100-test"))
					== args.end());
	bool levelsize20200 =
			!(args.find(std::string("--leveldb-size-20-200-test"))
					== args.end());
#endif

	if (bf || all) {
		BFTest bftest;
		bftest.run();
	}
	if (bdbinsert || all) {
		{
			BDBInsertRemoveTest bdbtest(DB_HASH);
			bdbtest.run();
		}
		{
			BDBInsertRemoveTest bdbtest(DB_BTREE);
			bdbtest.run();
		}

	}
	if (bdbtransaction || all) {
		{
			BDBTransactionsTest transaction(DB_BTREE);
			transaction.run();
		}
		{
			BDBTransactionsTest transaction(DB_HASH);
			transaction.run();
		}

	}
	if (bdb || all) {
		DBTest dbtest;
		dbtest.run();
	}

#ifdef HAVE_SQLITE
	if (sqlite || all) {
		SQLiteTest sqltest;
		sqltest.run();
	}
#endif

#ifdef HAVE_LEVELDB
	if (level || all) {
		{
			setsync::crypto::CryptoHash hash;
			LevelDbTest leveldb(hash);
			leveldb.run();
		}
		{
			setsync::crypto::CryptoHash hash("md5");
			LevelDbTest leveldb(hash);
			leveldb.run();
		}
	}
	if (levelsize || all) {
		setsync::crypto::CryptoHash hash;
		LevelDbTest leveldb(hash);
		leveldb.runDbSizeTestInSteps();
	} else {
		setsync::crypto::CryptoHash hash;
		if (levelsize8) {
			LevelDbTest leveldb(hash);
			leveldb.runDbSizeTestInSteps(8, 0);
		}
		if (levelsize16) {
			LevelDbTest leveldb(hash);
			leveldb.runDbSizeTestInSteps(16, 0);
		}
		if (levelsize20) {
			LevelDbTest leveldb(hash);
			leveldb.runDbSizeTestInSteps(20, 0);
		}
		if (levelsize20100) {
			LevelDbTest leveldb(hash);
			leveldb.runDbSizeTestInSteps(20, 100);
		}
		if (levelsize20200) {
			LevelDbTest leveldb(hash);
			leveldb.runDbSizeTestInSteps(20, 200);
		}
	}
#endif
	if (settest || all) {
		size_t elements = 100000;
		float fpr = ceil((float)(elements) * 0.000444595) / elements;
		size_t cachesize = 128 * 1024 * 1024;
#ifdef HAVE_DB_CXX_H
		cout << "Berkeley DB default cache" << endl;
		{
			setsync::utils::FileSystem::TemporaryDirectory
					tempdir("berkeleydb");
			SET_CONFIG cc = set_create_config();
			cc.bf_max_elements = elements;
			cc.false_positive_rate = fpr;
			cc.storage = BERKELEY_DB;
			setsync::config::Configuration c(cc);
			c.setPath(tempdir.getPath());
			evaluation::SetTest settest(c);
			settest.run();
		}
		cout << "Berkeley DB 128 MB cache" << endl;
		{
			setsync::utils::FileSystem::TemporaryDirectory
					tempdir("berkeleydb");
			SET_CONFIG cc = set_create_config();
			cc.bf_max_elements = elements;
			cc.false_positive_rate = fpr;
			cc.storage = BERKELEY_DB;
			cc.storage_cache_bytes = cachesize;
			cc.storage_cache_gbytes = 0;
			setsync::config::Configuration c(cc);
			c.setPath(tempdir.getPath());
			evaluation::SetTest settest(c);
			settest.run();
		}
		cout << "MemDB" << endl;
		{
			SET_CONFIG cc = set_create_config();
			cc.bf_max_elements = elements;
			cc.false_positive_rate = fpr;
			cc.storage = IN_MEMORY_DB;
			setsync::config::Configuration c(cc);
			evaluation::SetTest settest(c);
			settest.run();
		}
#endif
#ifdef HAVE_LEVELDB
		cout << "LevelDB default cache" << endl;
		{
			setsync::utils::FileSystem::TemporaryDirectory tempdir("leveldb");
			SET_CONFIG cc = set_create_config();
			cc.bf_max_elements = elements;
			cc.false_positive_rate = fpr;
			cc.storage = LEVELDB;
			setsync::config::Configuration c(cc);
			c.setPath(tempdir.getPath());
			evaluation::SetTest settest(c);
			settest.run();
		}
		cout << "LevelDB 128 MB cache" << endl;
		{
			setsync::utils::FileSystem::TemporaryDirectory tempdir("leveldb");
			SET_CONFIG cc = set_create_config();
			cc.bf_max_elements = elements;
			cc.false_positive_rate = fpr;
			cc.storage = LEVELDB;
			cc.storage_cache_bytes = cachesize;
			setsync::config::Configuration c(cc);
			c.setPath(tempdir.getPath());
			evaluation::SetTest settest(c);
			settest.run();
		}
#endif
	}

	if (speedtest || all) {
#ifdef HAVE_DB_CXX_H
		{
			cout << "running MemDB Speed Test with 1 GB RAM" << endl;
			size_t gbyte = 1;
			size_t byte = 0;
			setsync::storage::MemStorage storage(gbyte, byte);
			evaluation::SpeedTest mem(storage);
			mem.run();
			cout << "######################################" << endl;
		}
		{
			cout << "running Berkeley DB Speed Test with default RAM" << endl;
			setsync::utils::FileSystem::TemporaryDirectory tempdir("bdbspeed");
			string dbfile = tempdir.getPath() + "/test.db";
			Db db(NULL, 0);
			db.open(NULL, dbfile.c_str(), "test", DB_HASH, DB_CREATE, 0);
			{
				setsync::storage::BdbStorage storage(&db);
				evaluation::SpeedTest bdb(storage);
				bdb.run();
				cout << "######################################" << endl;
			}
			db.close(0);
		}
		{
			cout << "running Berkeley DB Speed Test with 128 MB RAM" << endl;
			setsync::utils::FileSystem::TemporaryDirectory tempdir("bdbspeed");
			string dbfile = tempdir.getPath() + "/test.db";
			Db db(NULL, 0);
			db.set_cachesize(0, 128 * 1024 * 1024, 0);
			db.open(NULL, dbfile.c_str(), "test", DB_HASH, DB_CREATE, 0);
			{
				setsync::storage::BdbStorage storage(&db);
				evaluation::SpeedTest bdb(storage);
				bdb.run();
				cout << "######################################" << endl;
			}
			db.close(0);
		}
#endif
#ifdef HAVE_LEVELDB
		{
			cout << "running LevelDB Speed Test with default RAM" << endl;
			setsync::utils::FileSystem::TemporaryDirectory tempdir(
					"leveldbspeed");
			setsync::storage::LevelDbStorage storage(tempdir.getPath());
			evaluation::SpeedTest bdb(storage);
			bdb.run();
			cout << "######################################" << endl;
		}
		{
			cout << "running LevelDB Speed Test with 128 MB RAM" << endl;
			std::size_t cachesize = 128 * 1024 * 1024;
			setsync::utils::FileSystem::TemporaryDirectory tempdir(
					"leveldbspeed");
			setsync::storage::LevelDbStorage storage(tempdir.getPath(),
					cachesize);
			evaluation::SpeedTest bdb(storage);
			bdb.run();
			cout << "######################################" << endl;
		}
#endif
	}
}
