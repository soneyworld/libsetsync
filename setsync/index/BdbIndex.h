/*
 * DbIndex.h
 *
 *      Author: Till Lorentzen
 */

#ifndef DBINDEX_H_
#define DBINDEX_H_

#include <setsync/BdbTableUser.h>
#include "AbstractSetIndex.h"
#include <db_cxx.h>

namespace setsync {

namespace index {
/**
 * This class is a berkeley DB based implementation of the
 * AbstractSetIndex. It puts the given key as key to a berkeley
 * DB hash map and stores the data as value.
 */
class BdbIndex: public berkeley::AbstractBdbTableUser,
		public setsync::index::AbstractSetIndex {
	friend class DbIndexTest;
private:
	/// Pointer to the berkeley db table, which should be used
	Db * db_;
public:
	/**
	 * Takes the given db pointer and uses it to save and load
	 * data to and from the berkeley db. The given hash function
	 * is used to get the size of the key in the berkeley db
	 *
	 * \param hash function to be used
	 * \param db pointer to an opened berkeley db table
	 */
	BdbIndex(const utils::CryptoHash& hash, Db * db);
	virtual ~BdbIndex();
	/**
	 * \return DB_HASH as table type
	 */
	static const DBTYPE getTableType() {
		return DB_HASH;
	}
	/**
	 * \return the name of the logical table: "setindex"
	 */
	static const char * getLogicalDatabaseName() {
		return "setindex";
	}
	/**
	 * Inserts the given data with the given length as
	 * value to the berkeley DB table and uses the hash
	 * as key.
	 *
	 * \param hash is the key to be inserted
	 * \param data which should be saved as value
	 * \param length of the data array
	 * \return true on success, otherwise false
	 */
	virtual bool put(const unsigned char * hash, const void * data,
			const std::size_t length);

	virtual size_t getSizeOf(const unsigned char * hash) const;
	virtual bool get(const unsigned char * hash, void * buffer,
			size_t * buffersize) const;

	virtual bool remove(const unsigned char * hash);

	virtual void clear(void);
};

}

}

#endif /* DBINDEX_H_ */
