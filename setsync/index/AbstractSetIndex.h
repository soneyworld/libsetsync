/*
 * AbstractSetIndex.h
 *
 *      Author: Till Lorentzen
 */

#ifndef ABSTRACTSETINDEX_H_
#define ABSTRACTSETINDEX_H_
#include <setsync/utils/CryptoHash.h>

namespace setsync {
/**
 * All classes in this namespace are used to save set entries
 * and its corresponding hashes in a hash map or such a data
 * structure.
 */
namespace index {
/**
 * The AbstractSetIndex provides the methods, used to be provided
 * by any SetIndex implementation. Implementations could be memory
 * only or on be saved on hard drive.
 *
 * The implementations provide the functionality to look for a
 * hash key and returns the saved value, if found.
 *
 * With the help of this class, all types of binary data with variable
 * length can be saved to the set and will also be returned.
 */
class AbstractSetIndex {
protected:
	/// Hash function used to create the key
	const utils::CryptoHash& hash_;
public:
	/**
	 * \param hash function to be used by the Index
	 */
	AbstractSetIndex(const utils::CryptoHash& hash);
	virtual ~AbstractSetIndex();
	/**
	 * Puts the given data with the given length to the index.
	 * The given data pointer must point to memory with the given
	 * length. If it is too large for holding all data in memory,
	 * mmap can be used to map the data to the memory space.
	 *
	 * \param data to be added
	 * \param length of the given data array
	 */
	virtual bool put(const void * data, const std::size_t length);

	/**
	 * Puts the given data with the given length to the index.
	 * The given data pointer must point to memory with the given
	 * length. If it is too large for holding all data in memory,
	 * mmap can be used to map the data to the memory space. The
	 * given hash identifies the given data.
	 *
	 * \param hash of the given data array
	 * \param data array to be added
	 * \param length of the given data array
	 */
	virtual bool put(const unsigned char * hash, const void * data,
			const std::size_t length) = 0;

	/**
	 * Requests the size of the stored value
	 *
	 * \param hash of the requested value
	 * \return the size of the value
	 */
	virtual size_t getSizeOf(const unsigned char * hash) const = 0;

	/**
	 * Requests the value of the passed hash and copies it into
	 * the given buffer with the given size. If there is no entry
	 * for the given hash false will be returned, otherwise, the
	 * length of the copied value will be returned by changing the
	 * buffersize variable.
	 *
	 * \param hash of the requested entry
	 * \param buffer where the result is saved to
	 * \param buffersize passes the maximum length of the buffer and returns the length of the value
	 * \return true, if value has been found
	 */
	virtual bool get(const unsigned char * hash, void * buffer,
			const size_t * buffersize) const = 0;

	/**
	 * Removes the entry in the index which has got the same
	 * hash, as the given one.
	 *
	 * \param hash of the entry, which should be removed
	 * \return true on success, otherwise false
	 */
	virtual bool remove(const unsigned char * hash) = 0;
	/**
	 * Deletes all entries in the index
	 */
	virtual void clear(void) = 0;
};

}

}

#endif /* ABSTRACTSETINDEX_H_ */
