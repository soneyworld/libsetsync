/*
 * KeyValueStorage.h
 *
 *      Author: Till Lorentzen
 */

#ifndef KEYVALUESTORAGE_H_
#define KEYVALUESTORAGE_H_
#include <setsync/utils/CryptoHash.h>
namespace setsync {

namespace storage {

class AbstractKeyValueStorage {
public:
	AbstractKeyValueStorage();
	virtual ~AbstractKeyValueStorage();
	virtual bool
	get(const unsigned char * key, const std::size_t length,
			unsigned char ** value, std::size_t * valueSize) const = 0;
	virtual void put(const unsigned char * key, const std::size_t keySize,
			const unsigned char * value, const std::size_t valueSize) = 0;
	virtual void del(const unsigned char * key, const std::size_t keySize) = 0;
	virtual void clear(void) = 0;
};

}

}

#endif /* KEYVALUESTORAGE_H_ */
