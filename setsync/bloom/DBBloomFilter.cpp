/*
 * DBBloomFilter.cpp
 *
 *      Author: Till Lorentzen
 */

#include "DBBloomFilter.h"

namespace bloom {

DBBloomFilter::DBBloomFilter() {
	Db db(NULL, 0);
	u_int32_t oFlags = DB_CREATE; // Open flags;
	try {
		// Open the database
		db.open(NULL,"my_db.db",NULL,DB_BTREE,oFlags,0);
	} catch (DbException &e) {
		// Error handling code goes here
	} catch (std::exception &e) {
		// Error handling code goes here
	}
}

DBBloomFilter::~DBBloomFilter() {
	// TODO Auto-generated destructor stub
}

}
