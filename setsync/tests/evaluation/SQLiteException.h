/*
 * SQLiteException.h
 *
 *      Author: Till Lorentzen
 */

#ifndef SQLITEEXCEPTION_H_
#define SQLITEEXCEPTION_H_
#include <exception>
#include <sqlite3.h>
namespace sqlite {

class SQLiteException: std::exception {
private:
	const char * msg;
public:
	SQLiteException(sqlite3 *db);
	const char* what(void) const throw ();
};

}

#endif /* SQLITEEXCEPTION_H_ */
