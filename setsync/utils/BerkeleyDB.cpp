/*
 * BerkeleyDB.cpp
 *
 *      Author: Till Lorentzen
 */

#include "BerkeleyDB.h"

namespace utils {

uint64_t BerkeleyDB::numberOfKeys(Db * db, DbTxn * txn, int flags) {
	DBTYPE dbtype;
	uint64_t nrecords = 0;
	int result = db->get_type(&dbtype);
	if (result != 0)
		throw DbException(result);
	switch (dbtype) {
	case DB_HASH:
		DB_HASH_STAT * hashstat;
		db->stat(txn, &hashstat, flags);
		nrecords = hashstat->hash_nkeys;
		delete hashstat;
		break;
	case DB_RECNO:
	case DB_BTREE:
		DB_BTREE_STAT *dbstat;
		db->stat(txn, &dbstat, flags);
		nrecords = dbstat->bt_nkeys;
		delete dbstat;
		break;
	case DB_QUEUE:
		DB_QUEUE_STAT * queuestat;
		db->stat(txn, &queuestat, flags);
		nrecords = queuestat->qs_nkeys;
		delete queuestat;
		break;
	default:
		break;
	}
	return nrecords;
}
uint64_t BerkeleyDB::numberOfKeys(Db * db) {
	return numberOfKeys(db, NULL, 0);
}

uint64_t BerkeleyDB::numberOfData(Db * db, DbTxn * txn, int flags) {
	DBTYPE dbtype;
	uint64_t nrecords = 0;
	int result = db->get_type(&dbtype);
	if (result != 0)
		throw DbException(result);
	switch (dbtype) {
	case DB_HASH:
		DB_HASH_STAT * hashstat;
		db->stat(txn, &hashstat, flags);
		nrecords = hashstat->hash_ndata;
		delete hashstat;
		break;
	case DB_RECNO:
	case DB_BTREE:
		DB_BTREE_STAT *dbstat;
		db->stat(txn, &dbstat, flags);
		nrecords = dbstat->bt_ndata;
		delete dbstat;
		break;
	case DB_QUEUE:
		DB_QUEUE_STAT * queuestat;
		db->stat(txn, &queuestat, flags);
		nrecords = queuestat->qs_ndata;
		delete queuestat;
		break;
	default:
		break;
	}
	return nrecords;
}

uint64_t BerkeleyDB::numberOfData(Db * db) {
	return numberOfData(db, NULL, 0);
}

std::string BerkeleyDB::tableTypeToString(Db * db) {
	DBTYPE type;
	db->get_type(&type);
	std::string t;
	switch (type) {
	case DB_HASH:
		t = "DB_HASH";
		break;
	case DB_BTREE:
		t = "DB_BTREE";
		break;
	case DB_QUEUE:
		t = "DB_QUEUE";
		break;
	case DB_RECNO:
		t = "DB_RECNO";
		break;
	default:
		t = "unknown type";
		break;
	}
	return t;
}

bool BerkeleyDB::isTransactionEnabled(Db * db) {
	DbEnv * env = db->get_env();
	if (env == NULL) {
		return false;
	}
	u_int32_t flags;
	env->get_flags(&flags);
	if ((flags & DB_INIT_TXN) == DB_INIT_TXN) {
		return true;
	} else {
		return false;
	}
}

}
