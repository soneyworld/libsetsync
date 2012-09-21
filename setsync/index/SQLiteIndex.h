/*
 * SQLiteIndex.h
 *
 *      Author: Till Lorentzen
 */

#ifndef SQLITEINDEX_H_
#define SQLITEINDEX_H_
#include <sqlite3.h>
#include <setsync/index/SQLiteDatabase.h>
#include <setsync/bloom/HashFunction.h>

namespace sqlite {
class SQLiteIndex {
private:
	sqlite3 * db_;
	sqlite3_stmt *insertStatement_;
public:
	SQLiteIndex(SQLiteDatabase *db, const std::string tablename);
	void insert(const uint64_t hash, const unsigned char * key, const size_t keylength);
	virtual ~SQLiteIndex();
};
}
#endif /* SQLITEINDEX_H_ */
