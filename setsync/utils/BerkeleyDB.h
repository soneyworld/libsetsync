/*
 * BerkeleyDB.h
 *
 *      Author: Till Lorentzen
 */

#ifndef BERKELEYDB_H_
#define BERKELEYDB_H_
#include <db_cxx.h>

namespace utils {
/**
 * Utilities and wrapper for berkeley DB operations
 */
class BerkeleyDB {
public:
	/**
	 * \param db which should be requested
	 * \param txn transaction pointer
	 * \param flags to be passed to db->stat()
	 * \return number of keys in the given db
	 */
	static uint64_t numberOfKeys(Db * db, DbTxn * txn, int flags);
	/**
	 * \param db which should be requested
	 * \return number of keys in the given db
	 */
	static uint64_t numberOfKeys(Db * db);
	/**
	 * \param db which should be requested
	 * \param txn transaction pointer
	 * \param flags to be passed to db->stat()
	 * \return number of data entries in the given db
	 */
	static uint64_t numberOfData(Db * db, DbTxn * txn, int flags);
	/**
	 * \param db which should be requested
	 * \return number of data entries in the given db
	 */
	static uint64_t numberOfData(Db * db);
};

}

#endif /* BERKELEYDB_H_ */
