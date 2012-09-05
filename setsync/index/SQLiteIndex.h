/*
 * SQLiteIndex.h
 *
 *      Author: Till Lorentzen
 */

#ifndef SQLITEINDEX_H_
#define SQLITEINDEX_H_
#include <sqlite3.h>

namespace index {
class SQLiteIndex {
private:
	 sqlite3 * db;
public:
	SQLiteIndex();
	virtual ~SQLiteIndex();
};
}
#endif /* SQLITEINDEX_H_ */
