/*
 * SQLiteDatabase.h
 *
 *      Author: Till Lorentzen
 */

#ifndef SQLITEDATABASE_H_
#define SQLITEDATABASE_H_
#include <sqlite3.h>
#include <string>
namespace sqlite{
class SQLiteDatabase {
private:
	 sqlite3 * db;
public:
	SQLiteDatabase();
	SQLiteDatabase(const std::string filename);
	sqlite3 * getConnection();
	virtual ~SQLiteDatabase();
};
}
#endif /* SQLITEDATABASE_H_ */
