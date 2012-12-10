/*
 * Set.hpp
 *
 *      Author: Till Lorentzen
 */

#ifndef SET_HPP_
#define SET_HPP_
#include "set.h"
#include <stdint.h>
#include <setsync/storage/KeyValueStorage.h>
#include <setsync/bloom/KeyValueCountingBloomFilter.h>
#include <setsync/index/KeyValueIndex.h>
#include <setsync/trie/KeyValueTrie.h>
#include <setsync/utils/CryptoHash.h>
#include <setsync/config/Configuration.h>
#include <setsync/utils/FileSystem.h>
#include <setsync/Synchronization.h>
#ifdef HAVE_DB_CXX_H
#include <db_cxx.h>
#endif

namespace setsync {
/**
 * Main class to use for a set, which should be synchronized
 * over a network or locally. It provides mechanisms to add, find
 * and remove set members. Members are only cryptographic hash
 * keys or binary data, which is hashed automatically on insertion
 * or find.
 */
class Set {
	friend class AbstractSynchronizationProcess;
private:
	/// maximum number of allowed set elements
	size_t maxSize_;
	/// true if the maximum is a hard limit
	bool hardMaximum_;
	/// The used cryptographic hash function
	const utils::CryptoHash hash_;
	/// The used configuration
	const config::Configuration& config_;
	/// A bloom filter instance of the set
	bloom::KeyValueCountingBloomFilter * bf_;
	/// A trie data structure of the set
	trie::KeyValueTrie * trie_;
	/// A storage in which binary data is saved, if it has been given
	index::KeyValueIndex * index_;
	/// Key value store for the bloom filter
	setsync::storage::AbstractKeyValueStorage * bfStorage_;
	/// Key value store for the trie data structure
	setsync::storage::AbstractKeyValueStorage * trieStorage_;
	/// Key value store for the binary data index
	setsync::storage::AbstractKeyValueStorage * indexStorage_;
	/// If no directory is given, a temporary directory is created
	utils::FileSystem::TemporaryDirectory * tempDir;
	/// true if more than one binary element is available
	bool indexInUse_;
#ifdef HAVE_DB_CXX_H
	/// The Berkeley DB environment used by bloom filter and trie
	DbEnv * env_;
	/// DB pointer for the bloom filter
	Db * bfdb;
	/// DB pointer for the trie
	Db * triedb;
	/// DB pointer for the index
	Db * indexdb;
#endif
protected:
	/**
	 * Sets the number of elements, added to this set
	 *
	 * \param s number of elements
	 */
	void setSize(const size_t s);
public:
	/**
	 * Creates a new set from the given Configuration. If no path
	 * is set in the config, a temporary directory is created and
	 * is automatically deleted after destroying this set. If a not
	 * supported key value storage type is given, an exception is
	 * thrown. Otherwise three key value stores and a bloom filter
	 * file is created at the path. If the path exists the set will
	 * try to load the set from the given path. If no set exists, a
	 * new set will be created. All database files will be available
	 * after destroying the set.
	 *
	 * \param config to be used for the set
	 */
	Set(const config::Configuration& config);
	/**
	 * \return true, if no element is in the set
	 */
	bool isEmpty() const;
	/**
	 * \return the number of elements in the set
	 */
	size_t getSize() const;
	/**
	 * The maximum of a set can be given in the configuration at
	 * the initialization. It cannot be changed while the instance
	 * is available. But if the maximum is not a hard limit, there
	 * is no check, if the limit is reached. But the synchronization
	 * performance is not a well as it could be.
	 *
	 * \return the maximum limit of the set
	 */
	size_t getMaximumSize() const;
	// Lookup for existence
	virtual bool find(const unsigned char * key);
	virtual bool find(const char * str);
	virtual bool find(const std::string& str);
	virtual bool find(const void * data, const std::size_t length);

	// Modifiers
	/**
	 * Add a cryptographic key to the set. It will just be copied to
	 * the set.
	 *
	 * \param key to be added
	 * \return true on success
	 */
	virtual bool insert(const unsigned char * key);
	/**
	 * Add a string to the set. It will hash the string and will add
	 * the hash to the set. The string won't be stored to the set
	 * and is not recoverable.
	 *
	 * \param str which should be hashed and its hash will be added
	 * \return true on success
	 */
	virtual bool insert(const char * str);
	/**
	 * Add a string to the set. It will hash the string and will add
	 * the hash to the set. The string won't be stored to the set
	 * and is not recoverable.
	 *
	 * \param str which should be hashed and its hash will be added
	 * \return true on success
	 */
	virtual bool insert(const std::string& str);
	/**
	 * Add binary data with the given length to the set. It will be
	 * hashed and the hash will be added to the set. But also the
	 * given binary data will be saved in the set
	 *
	 * \param data pointer to binary data, which should be added
	 * \param length of the data
	 * \return true on success
	 */
	virtual bool insert(const void * data, const std::size_t length);
	/**
	 * Removes the given key from the set. It also removes binary
	 * data from the set, which has the same hash.
	 *
	 * \param key
	 * \return true on success
	 */
	virtual bool erase(const unsigned char * key);
	/**
	 * Hashes the given string and removes the hash of the string from
	 * the set. It also removes binary data from the set, which has the
	 * same hash.
	 *
	 * \param str to be removed from set
	 * \return true on success
	 */
	virtual bool erase(const char * str);
	/**
	 * Hashes the given string and removes the hash of the string from
	 * the set. It also removes binary data from the set, which has the
	 * same hash.
	 *
	 * \param str to be removed from set
	 * \return true on success
	 */
	virtual bool erase(const std::string& str);
	/**
	 * Hashes the given binary data and removes the hash of it from
	 * the set. It also removes binary data from the set, which has the
	 * same hash.
	 *
	 * \param str to be removed from set
	 * \return true on success
	 */
	virtual bool erase(const void * data, const std::size_t length);
	/**
	 * Deletes all elements from the set
	 */
	virtual void clear();
	/**
	 * Destroys the set instance. It also removes all key value stores,
	 * if no path has been set to the used configuration. If a path
	 * has been set, the set stays untouched on the given path and
	 * could be loaded by a new instance by passing the same path to the
	 * new instance.
	 */
	virtual ~Set();
};
}

#endif /* SET_HPP_ */
