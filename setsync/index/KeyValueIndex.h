/*
 * KeyValueIndex.h
 *
 *      Author: Till Lorentzen
 */

#ifndef KEYVALUEINDEX_H_
#define KEYVALUEINDEX_H_

#include "AbstractSetIndex.h"
#include <setsync/utils/CryptoHash.h>
#include <setsync/storage/KeyValueStorage.h>

namespace setsync {

namespace index {

class KeyValueIndex: public setsync::index::AbstractSetIndex {
private:
	setsync::storage::AbstractKeyValueStorage& storage_;
public:
	KeyValueIndex(const utils::CryptoHash& hash, setsync::storage::AbstractKeyValueStorage& storage);
	virtual ~KeyValueIndex();
	virtual void clear();
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
