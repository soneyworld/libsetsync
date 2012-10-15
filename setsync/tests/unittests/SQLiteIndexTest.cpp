/*
 * SQLiteIndexTest.cpp
 *
 *      Author: Till Lorentzen
 */

#include "SQLiteIndexTest.h"
#include "setsync/index/SQLiteDatabase.h"
#include "setsync/sha1.h"
namespace sqlite{
void SQLiteIndexTest::setUp(void) {
	std::string filename = "test.db";
	std::string tablename = "revindex";
	this->db = new sqlite::SQLiteDatabase(filename);
	this->index = new sqlite::SQLiteIndex(this->db, tablename);
}
void SQLiteIndexTest::tearDown(void) {
	delete this->index;
	delete this->db;
	remove("test.db");
}
void SQLiteIndexTest::testAddingAndErasingElements() {

}
void SQLiteIndexTest::testAddingAndCleaningElements() {
	bloom::SaltedHashFunction f(10);
	unsigned char md[20];
	SHA1((const unsigned char *) "hello", 5, md);
	for (std::size_t i = 0; i < f.count(); i++) {
		this->index->insert(f.hash(md, 20, i), md, 20);
	}
}
void SQLiteIndexTest::testAddingAndFindingElements() {

}
};
