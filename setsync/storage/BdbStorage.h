/*
 * BdbStorage.h
 *
 *      Author: Till Lorentzen
 */

#ifndef BDBSTORAGE_H_
#define BDBSTORAGE_H_

#include "KeyValueStorage.h"
#include <db_cxx.h>
#include <setsync/storage/BdbTableUser.h>

namespace setsync {

namespace storage {

/**
 * Implementation of an abstract iterator to provide the
 * possibility to iterate over a berkeley db
 */
class BdbIterator: public AbstractKeyValueIterator {
private:
	/// The berkeley db cursor to be used
	Dbc *cursorp;
	DbTxn * txn;
	bool valid_;
	Dbt key_;
	Dbt data_;
public:
	/**
	 * Creates a new instance of an iterator Wrapper for
	 * leveldb::Iterator
	 *
	 * \param db to be used
	 */
	BdbIterator(Db * db, DbTxn * parent = NULL);
	virtual ~BdbIterator();
	virtual void seekToFirst();
	virtual bool valid() const;
	virtual void next();
	virtual size_t keySize() const;
	virtual void key(unsigned char * buffer) const;
};

class BdbStorage: public setsync::storage::AbstractKeyValueStorage,
		public berkeley::AbstractBdbTableUser {
private:
	Db * db_;
public:
	/**
	 * Uses the given berkeley db as key value store
	 */
	BdbStorage(Db * db);
	/**
	 * Closes the connection to the key value store
	 */
	virtual ~BdbStorage();
	virtual bool get(const unsigned char * key, const std::size_t length,
			unsigned char ** value, std::size_t * valueSize) const;
	virtual void put(const unsigned char * key, const std::size_t keySize,
			const unsigned char * value, const std::size_t valueSize);
	virtual void del(const unsigned char * key, const std::size_t keySize);
	/**
	 * Closes the connection to the store, deletes all files
	 * and reopens a new connection to the same path
	 */
	virtual void clear(void);
	virtual AbstractKeyValueIterator * createIterator();
};

}

}

#endif /* BDBSTORAGE_H_ */
