/*
 * SQLiteDatabase.cpp
 *
 *      Author: Till Lorentzen
 */

#include "SQLiteDatabase.h"
#include "SQLiteException.h"

namespace sqlite{

SQLiteDatabase::SQLiteDatabase() {
	int rc = sqlite3_open_v2(":memory:", &db,
			SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_URI,
			"unix-none");
	if (rc) {
		SQLiteException e = SQLiteException(db);
		sqlite3_close(db);
		throw e;
	}
}

SQLiteDatabase::SQLiteDatabase(const std::string filename) {
	int rc = sqlite3_open_v2(filename.c_str(), &db,
			SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_URI,
			"unix-dotfile");
	if (rc) {
		SQLiteException e = SQLiteException(db);
		sqlite3_close(db);
		throw e;
	}
}

sqlite3 * SQLiteDatabase::getConnection(){
	return this->db;
}

SQLiteDatabase::~SQLiteDatabase() {
	sqlite3_close(db);
}
}
