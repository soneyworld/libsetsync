/*
 * Main.cpp
 *
 *      Author: Till Lorentzen
 */
#include "config.h"
#include "BFTest.h"
#include "DBTest.h"
#ifdef HAVE_SQLITE
#include "SQLiteTest.h"
#endif

int main(int ac, char **av) {
	BFTest bftest;
	bftest.run();

	DBTest dbtest;
	dbtest.run();

#ifdef HAVE_SQLITE
//	SQLiteTest sqltest;
//	sqltest.run();
#endif


}
