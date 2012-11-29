/*
 * DbIndexTest.cpp
 *
 *      Author: Till Lorentzen
 */

#include "DbIndexTest.h"

namespace setsync {

namespace index {

void DbIndexTest::setUp(void) {
	this->db = new Db(NULL, 0);
	db->open(NULL, "index.db", DbIndex::getLogicalDatabaseName(),
			DbIndex::getTableType(), DB_CREATE, 0);
	index = new DbIndex(hash,db);
}

void DbIndexTest::tearDown(void) {
	delete this->index;
	db->close(0);
	delete this->db;
	this->db = new Db(NULL, 0);
	this->db->remove("index.db", NULL, 0);
	delete this->db;
}

void DbIndexTest::testAdding(void) {

}
}

}
