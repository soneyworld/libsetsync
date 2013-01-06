/*
 * MemStorage.h
 *
 *      Author: Till Lorentzen
 */

#ifndef MEMSTORAGE_H_
#define MEMSTORAGE_H_

#include "KeyValueStorage.h"
#include <setsync/storage/BdbStorage.h>
namespace setsync {

namespace storage {

class MemStorage: public setsync::storage::AbstractKeyValueStorage {
private:
	Db * inMemoryDb_;
	BdbStorage * storage_;
public:
	MemStorage(const std::size_t gbcache, const std::size_t cache);
	virtual ~MemStorage();
	virtual bool get(const unsigned char * key, const std::size_t length,
			unsigned char ** value, std::size_t * valueSize) const;
	virtual void put(const unsigned char * key, const std::size_t keySize,
			const unsigned char * value, const std::size_t valueSize);
	virtual void del(const unsigned char * key, const std::size_t keySize);
	/**
	 * Cleans map and frees all memory
	 */
	virtual void clear(void);
	virtual AbstractKeyValueIterator * createIterator();
};

}

}

#endif /* MEMSTORAGE_H_ */
