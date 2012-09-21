/*
 * SQLiteIndex.cpp
 *
 *      Author: Till Lorentzen
 */

#include "SQLiteIndex.h"
#include "SQLiteException.h"
namespace sqlite {

SQLiteIndex::SQLiteIndex(SQLiteDatabase * db, const std::string tablename) {
	this->db_ = db->getConnection();
	/*	sqlite3_stmt *statement;

	 if (sqlite3_prepare_v2(this->db_, "CREATE TABLE IF NOT EXISTS rindex (hash INTEGER, md BLOB);",
	 -1, &statement, 0) == SQLITE_OK) {
	 int cols = sqlite3_column_count(statement);
	 int result = 0;
	 while (true) {
	 result = sqlite3_step(statement);

	 if (result == SQLITE_ROW) {
	 for (int col = 0; col < cols; col++) {
	 std::string s = (char*) sqlite3_column_text(statement, col);
	 //do something with it
	 }
	 } else {
	 break;
	 }
	 }

	 sqlite3_finalize(statement);
	 }*/
	int rc = sqlite3_exec(this->db_,
			"CREATE TABLE IF NOT EXISTS revindex (hash INTEGER,md BLOB);",
			NULL, NULL, NULL);
	if (rc) {
		SQLiteException e = SQLiteException(this->db_);
		throw e;
	}
	rc = sqlite3_prepare_v2(this->db_, "INSERT INTO revindex VALUES (?,?);",
			-1, &insertStatement_, 0);

}

void SQLiteIndex::insert(const uint64_t hash, const unsigned char * key,
		const size_t keylength) {
	int rc;
	const sqlite3_uint64 sqlitehash = hash;
	rc = sqlite3_bind_int64(this->insertStatement_, 1, sqlitehash);
	rc = sqlite3_bind_blob(this->insertStatement_, 2, key, keylength,
			SQLITE_STATIC);
	rc = sqlite3_step(this->insertStatement_);
}

SQLiteIndex::~SQLiteIndex() {
	sqlite3_finalize(this->insertStatement_);
}
}
