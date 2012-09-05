/*
 * SQLiteIndex.cpp
 *
 *      Author: Till Lorentzen
 */

#include "SQLiteIndex.h"
#include "SQLiteException.h"
namespace index {

SQLiteIndex::SQLiteIndex() {
	int rc = sqlite3_open_v2(":memory:", &db,
			SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_URI,
			"unix-none");
	if (rc) {
		SQLiteException e = SQLiteException(db);
		sqlite3_close(db);
		throw e;
	}
}

SQLiteIndex::SQLiteIndex(const char * filename) {
	int rc = sqlite3_open_v2(filename, &db,
			SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_URI,
			"unix-dotfile");
	if (rc) {
		SQLiteException e = SQLiteException(db);
		sqlite3_close(db);
		throw e;
	}
}

SQLiteIndex::~SQLiteIndex() {
	sqlite3_close(db);
}
}
