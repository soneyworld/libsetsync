/*
 * SQLiteIndex.cpp
 *
 *      Author: Till Lorentzen
 */

#include "SQLiteIndex.h"
#include "SQLiteException.h"
namespace index{


SQLiteIndex::SQLiteIndex() {
	int rc = sqlite3_open("file:index.db", &db);
	if( rc ){
		SQLiteException e = SQLiteException(db);
		sqlite3_close(db);
		throw e;
	}
}

SQLiteIndex::~SQLiteIndex() {
	sqlite3_close(db);
}
}
