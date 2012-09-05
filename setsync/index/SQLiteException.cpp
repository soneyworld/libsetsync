/*
 * SQLiteException.cpp
 *
 *      Author: Till Lorentzen
 */

#include "SQLiteException.h"

namespace index {

SQLiteException::SQLiteException(sqlite3 * db) {
	this->msg = sqlite3_errmsg(db);

}

const char* SQLiteException::what(void) const throw (){
	return this->msg;
}
}
