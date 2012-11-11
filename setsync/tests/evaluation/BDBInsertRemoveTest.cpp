/*
 * BDBInsertRemoveTest.cpp
 *
 *      Author: Till Lorentzen
 */

#include "BDBInsertRemoveTest.h"
#include <setsync/utils/FileSystem.h>

using namespace std;

BDBInsertRemoveTest::BDBInsertRemoveTest(const DBTYPE type) :
	type_(type) {
	remove("temp-table.db");
	this->db = new Db(NULL, 0);
	db->open(NULL, "temp-table.db", "test", type, DB_CREATE, 0);
}

BDBInsertRemoveTest::~BDBInsertRemoveTest() {
	db->close(0);
	delete db;
	remove("temp-table.db");
}

void BDBInsertRemoveTest::run() {
	string t;
	switch (type_) {
	case DB_HASH:
		t = "DB_HASH";
		break;
	case DB_BTREE:
		t = "DB_BTREE";
		break;
	default:
		t = "unknown type";
		break;
	}
	cout << "running Berkeley DB insert delete file size test (" << t << ")"
			<< endl;
	fillDB();
	cout << ITERATIONS << " inserts done:\nITEMS_PER_LOOP=" << ITEMS_PER_LOOPS
			<< endl;
	cout << "LOOP_ITERATIONS="<<LOOP_ITERATIONS<< endl;
	insertRemove();
}

void BDBInsertRemoveTest::insertRemove() {
	cout << "loop,noentries,filesize" << endl;
	Dbt value(NULL, 0);
	cout << "0," << getNumberOfEntries() << "," << utils::FileSystem::fileSize(
			"temp-table.db") << endl;
	for (uint64_t loop = 0; loop < LOOP_ITERATIONS; loop++) {
		for (uint64_t item = 0; item < ITEMS_PER_LOOPS; item++) {
			uint64_t d = loop * ITEMS_PER_LOOPS + item;
			uint64_t i = loop * ITEMS_PER_LOOPS + item + ITERATIONS;
			Dbt dkey(&d, sizeof(uint64_t));
			Dbt ikey(&i, sizeof(uint64_t));
			this->db->del(NULL, &dkey, 0);
			this->db->put(NULL, &ikey, &value, 0);
		}
		this->db->sync(0);
		cout << loop + 1 << "," << getNumberOfEntries() << ","
				<< utils::FileSystem::fileSize("temp-table.db") << endl;
	}
	for (uint64_t loop = 0; loop < LOOP_ITERATIONS; loop++) {
		for (uint64_t item = 0; item < ITEMS_PER_LOOPS; item++) {
			uint64_t d = loop * ITEMS_PER_LOOPS + item + ITERATIONS;
			Dbt dkey(&d, sizeof(uint64_t));
			this->db->del(NULL, &dkey, 0);
		}
		this->db->compact(NULL,NULL,NULL,NULL,DB_FREE_SPACE,NULL);
		this->db->sync(0);
		cout << loop + 1 + LOOP_ITERATIONS << "," << getNumberOfEntries()
				<< "," << utils::FileSystem::fileSize("temp-table.db") << endl;
	}
}

void BDBInsertRemoveTest::fillDB() {
	Dbt value(NULL, 0);
	for (uint64_t i = 0; i < ITERATIONS; i++) {
		Dbt key(&i, sizeof(uint64_t));
		this->db->put(NULL, &key, &value, 0);
	}
	this->db->sync(0);
}

unsigned int BDBInsertRemoveTest::getNumberOfEntries() {
	unsigned int nrecords;
	switch (type_) {
	case DB_HASH:
		DB_HASH_STAT * hashstat;
		db->stat(NULL, &hashstat, 0);
		nrecords = hashstat->hash_nkeys;
		free(hashstat);
		break;
	case DB_BTREE:
		DB_BTREE_STAT *dbstat;
		db->stat(NULL, &dbstat, 0);
		nrecords = dbstat->bt_nkeys;
		free(dbstat);
		break;
	default:
		exit(-1);
	}
	return nrecords;
}

