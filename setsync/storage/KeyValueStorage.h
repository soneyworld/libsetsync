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
/**
 * Iterator for AbstractKeyValueStorage to iterate over
 * its entries
 */
class AbstractKeyValueIterator {
public:
	/**
	 * Resets the Iterator position to the first position
	 */
	virtual void seekToFirst() = 0;
	/**
	 * \returns true if this entry exists, otherwise false
	 */
	virtual bool valid() const = 0;
	/**
	 * Set the iterator position to the next entry
	 */
	virtual void next() = 0;
	/**
	 * \return the size in bytes of the actual key
	 */
	virtual size_t keySize() const = 0;
	/**
	 * Copies the actual key with the keySize() to a given
	 * allocated memory
	 *
	 * \param buffer where the key should be copied to
	 */
	virtual void key(unsigned char * buffer) const = 0;
};
/**
 * A generic interface for key-value storages
 */
class AbstractKeyValueStorage {
public:
	AbstractKeyValueStorage();
	virtual ~AbstractKeyValueStorage();
	/**
	 * Copies the result of the search for the given key
	 * into a newly allocated memory position and writes the
	 * pointer to value, if the search has been successful.
	 * The size of the allocated memory will be passed to
	 * valueSize.
	 * If no entry has been found, the valueSize is set to 0,
	 * the value pointing is NULL and false is returned.
	 *
	 * \param key to be searched for
	 * \param length of the given key
	 * \param value pointer to the stored value
	 * \param valueSize of the returned value
	 * \return true on success, otherwise false
	 */
	virtual bool
	get(const unsigned char * key, const std::size_t length,
			unsigned char ** value, std::size_t * valueSize) const = 0;
	/**
	 * Adds or updates the key and its value
	 *
	 * \param key to be saved
	 * \param keySize of the key in bytes
	 * \param value to be saved
	 * \param valueSize of the value in bytes
	 */
	virtual void put(const unsigned char * key, const std::size_t keySize,
			const unsigned char * value, const std::size_t valueSize) = 0;
	/**
	 * Deletes the entry of the given key
	 *
	 * \param key to be deleted
	 * \param keySize of the given key
	 */
	virtual void del(const unsigned char * key, const std::size_t keySize) = 0;
	/**
	 * Deletes the complete storage. There are no entries available
	 * after this call
	 */
	virtual void clear(void) = 0;
	/**
	 * To iterate over the complete key value store, this
	 * method provides an iterator to perform such a search.
	 *
	 * \return a new iterator
	 */
	virtual AbstractKeyValueIterator * createIterator() = 0;
};

}

}

#endif /* KEYVALUESTORAGE_H_ */
