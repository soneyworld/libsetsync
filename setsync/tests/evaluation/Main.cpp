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
#ifdef HAVE_LEVELDB
#include "LevelDbTest.h"
#endif
#include <string>
#include <set>

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
#ifdef HAVE_SQLITE
		std::cout << "--sql-test" << std::endl;
#endif
#ifdef HAVE_LEVELDB
		std::cout << "--leveldb-test" << std::endl;
		std::cout << "--leveldb-size-test" << std::endl;
#endif
		std::cout << "--all-tests" << std::endl;
		return 0;
	}
	bool bdb = !(args.find(std::string("--bdb-test")) == args.end());
	bool bf = !(args.find(std::string("--bf-test")) == args.end());
	bool bdbinsert = !(args.find(std::string("--bdb-insert-remove-test"))
			== args.end());
	bool bdbtransaction = !(args.find(std::string("--bdb-transaction-test"))
			== args.end());
	bool all = !(args.find(std::string("--all-tests")) == args.end());
#ifdef HAVE_SQLITE
	bool sqlite = !(args.find(std::string("--sql-test")) == args.end());
#endif
#ifdef HAVE_LEVELDB
	bool level = !(args.find(std::string("--leveldb-test")) == args.end());
	bool levelsize = !(args.find(std::string("--leveldb-size-test")) == args.end());
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
			utils::CryptoHash hash;
			LevelDbTest leveldb(hash);
			leveldb.run();
		}
		{
			utils::CryptoHash hash("md5");
			LevelDbTest leveldb(hash);
			leveldb.run();
		}
	}
	if(levelsize || all) {
		utils::CryptoHash hash;
		LevelDbTest leveldb(hash);
		leveldb.runDbSizeTestInSteps();
	}

#endif
}
