/*
 * SQLiteDatabase.cpp
 *
 *      Author: Till Lorentzen
 */

#include "SQLiteDatabase.h"

SQLiteDatabase::SQLiteDatabase() {
	int rc = sqlite3_open("hashes.db",&db);
	if (rc) {
		sqlite3_close(db);
		throw "Datenbankverbindung fehlgeschlagen";
	}
}

unsigned int SQLiteDatabase::getMaximum(std::string hashname) {
	"SELECT name FROM sqlite_master WHERE type='table' AND name='table_name'";
	return 0;
}

SQLiteDatabase::~SQLiteDatabase() {
	sqlite3_close(db);
}
