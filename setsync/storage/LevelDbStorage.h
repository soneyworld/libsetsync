/*
 * LevelDbStorage.h
 *
 *      Author: Till Lorentzen
 */

#ifndef LEVELDBSTORAGE_H_
#define LEVELDBSTORAGE_H_

#include "KeyValueStorage.h"
#include <leveldb/db.h>

namespace setsync {

namespace storage {

/**
 * Implementation of an abstract iterator to provide the
 * possibility to iterate over a LevelDbStorage
 */
class LevelDbIterator: public AbstractKeyValueIterator {
private:
	/// The level db iterator to be used
	leveldb::Iterator* it;
public:
	/**
	 * Creates a new instance of an iterator Wrapper for
	 * leveldb::Iterator
	 *
	 * \param it the leveldb::Iterator, which should be used
	 */
	LevelDbIterator(leveldb::Iterator* it);
	virtual ~LevelDbIterator();
	virtual void seekToFirst();
	virtual bool valid() const;
	virtual void next();
	virtual size_t keySize() const;
	virtual void key(unsigned char * buffer) const;
};

/**
 * A simple implementation of the abstract key value storage
 * by using the leveldb implementation.
 */
class LevelDbStorage: public setsync::storage::AbstractKeyValueStorage {
	friend class LevelDbStorageTest;
private:
	/// Instance of the table to be used
	leveldb::DB * db_;
	/// Path where the db exists
	std::string path_;
	/// Database options
	leveldb::Options options_;
	/// Options for write accesses
	leveldb::WriteOptions writeOptions_;
	/// Options for read accesses
	leveldb::ReadOptions readOptions_;
	/**
	 *
	 */
	void init();
public:
	/**
	 * Creates a new key value store at the given path, or
	 * loads an existing store.
	 *
	 * \param path where the database exists, or should be saved to
	 */
	LevelDbStorage(const std::string& path);
	/**
	 * Creates a new key value store at the given path, or
	 * loads an existing store. The given Cache size is used.
	 *
	 * \param path where the database exists, or should be saved to
	 * \param cache_capacity with can be used
	 */
	LevelDbStorage(const std::string& path, const std::size_t cache_capacity);
	/**
	 * Creates a new key value store at the given path, or
	 * loads an existing store.
	 *
	 * \param path where the database exists, or should be saved to
	 * \param options will be used for this storage
	 */
	LevelDbStorage(const std::string& path, const leveldb::Options& options);
	/**
	 * Creates a new key value store at the given path, or
	 * loads an existing store.
	 */
	LevelDbStorage(const std::string& path, const leveldb::Options& options,
			const leveldb::WriteOptions& woptions);
	/**
	 * Creates a new key value store at the given path, or
	 * loads an existing store.
	 */
	LevelDbStorage(const std::string& path, const leveldb::Options& options,
			const leveldb::ReadOptions& roptions);
	/**
	 * Creates a new key value store at the given path, or
	 * loads an existing store.
	 */
	LevelDbStorage(const std::string& path, const leveldb::Options& options,
			const leveldb::ReadOptions& roptions,
			const leveldb::WriteOptions& woptions);


	/**
	 * Closes the connection to the key value store
	 */
	virtual ~LevelDbStorage();
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

#endif /* LEVELDBSTORAGE_H_ */
