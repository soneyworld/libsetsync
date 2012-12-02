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

class LevelDbStorage: public setsync::storage::AbstractKeyValueStorage {
	friend class LevelDbStorageTest;
private:
	leveldb::DB * db_;
	std::string path_;
	leveldb::Options options_;
	leveldb::WriteOptions writeOptions_;
	leveldb::ReadOptions readOptions_;
public:
	LevelDbStorage(const std::string& path);
	virtual ~LevelDbStorage();
	virtual bool get(const unsigned char * key, const std::size_t length,
			unsigned char ** value, std::size_t * valueSize) const;
	virtual void put(const unsigned char * key, const std::size_t keySize,
			const unsigned char * value, const std::size_t valueSize);
	virtual void del(const unsigned char * key, const std::size_t keySize);
	virtual void clear(void);
};

}

}

#endif /* LEVELDBSTORAGE_H_ */
