/*
 * KeyValueIndex.h
 *
 *      Author: Till Lorentzen
 */

#ifndef KEYVALUEINDEX_H_
#define KEYVALUEINDEX_H_

#include "AbstractSetIndex.h"
#include <setsync/crypto/CryptoHash.h>
#include <setsync/storage/KeyValueStorage.h>

namespace setsync {

namespace index {
/**
 * The key value index will save the given hashes as keys and the
 * corresponding data as value to the given key value storage.
 * It can be used to save also the data of a set, not only the
 * calculated hashes. This can be helpful, if the user of a set
 * also wants to save the data entries and read from them. Or to
 * save the path to a file, which contains the hashed data.
 */
class KeyValueIndex: public setsync::index::AbstractSetIndex {
private:
	/// storage to be used for index entries
	setsync::storage::AbstractKeyValueStorage& storage_;
public:
	/**
	 *
	 * \param storage to save the hashes and the values
	 * \param hash function which has been used to calculate the hashes of values to be saved
	 */
	KeyValueIndex(const crypto::CryptoHash& hash,
			setsync::storage::AbstractKeyValueStorage& storage);
	virtual ~KeyValueIndex();
	/**
	 * Deletes all entries in the given storage
	 */
	virtual void clear();
	/**
	 *
	 */
	virtual bool remove(const unsigned char* hash);
	virtual size_t getSizeOf(const unsigned char * hash) const;
	virtual bool get(const unsigned char * hash, void * buffer,
			size_t * buffersize) const;
	virtual bool put(const unsigned char * hash, const void * data,
			const std::size_t length);
};

}

}

#endif /* KEYVALUEINDEX_H_ */
