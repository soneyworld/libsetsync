/*
 * Main.cpp
 *
 *      Author: Till Lorentzen
 */
#include "config.h"
#include "BFTest.h"
#include "DBTest.h"
#include <string>
#include <set>

#ifdef HAVE_SQLITE
#include "SQLiteTest.h"
#endif

int main(int ac, char **av) {
	std::set<std::string> args;
	std::set<std::string>::iterator iter;
	for (int i = 0; i < ac; i++) {
		std::string arg(av[i]);
		args.insert(arg);
	}

	if (!(args.find(std::string("--bf-test"))==args.end())) {
		BFTest bftest;
		bftest.run();
	}

	if (!(args.find(std::string("--bdb-test"))==args.end())) {
		DBTest dbtest;
		dbtest.run();
	}

#ifdef HAVE_SQLITE
	if (!(args.find(std::string("--sql-test"))==args.end())) {
		SQLiteTest sqltest;
		sqltest.run();
	}
#endif

}
