/*
 * SQLiteDatabase.h
 *
 *      Author: Till Lorentzen
 */

#ifndef SQLITEDATABASE_H_
#define SQLITEDATABASE_H_
#include <sqlite3.h>
#include <string>

class SQLiteDatabase {
private:
	 sqlite3 * db;
public:
	SQLiteDatabase();
	unsigned int getMaximum(std::string hashname);
	virtual ~SQLiteDatabase();
};

#endif /* SQLITEDATABASE_H_ */
