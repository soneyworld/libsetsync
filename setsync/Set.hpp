/*
 * Set.hpp
 *
 *      Author: Till Lorentzen
 */

#ifndef SET_HPP_
#define SET_HPP_
#include "set.h"
#include "config.h"
#include <stdint.h>
#include <setsync/sync/Synchronization.h>
#include <setsync/storage/KeyValueStorage.h>
#include <setsync/bloom/KeyValueCountingBloomFilter.h>
#include <setsync/index/KeyValueIndex.h>
#include <setsync/trie/KeyValueTrie.h>
#include <setsync/crypto/CryptoHash.h>
#include <setsync/config/Configuration.h>
#include <setsync/utils/FileSystem.h>
#include <queue>
#include <stack>

#ifdef HAVE_DB_CXX_H
#include <db_cxx.h>
#endif

namespace setsync {

/**
 * With the help of this class, an external set can be synchronized with
 * the local instance.
 */
class SynchronizationProcess {
	friend class SetTest;
private:
	enum status {
		START, BF, TRIE, EQUAL
	} stat_;
	/// Local set instance
	Set * set_;
	/// default handler, C++ interface only, NULL if the no diff handler should be used
	AbstractDiffHandler * handler_;
	std::size_t bloomfilterOut_pos;
	std::size_t bloomfilterIn_pos;
	bool bfOutIsFinished_;
	std::size_t sentBytes_;
	std::size_t receivedBytes_;
	std::queue<crypto::CryptoHashContainer> sentHashes_;
	std::stack<crypto::CryptoHashContainer> pendingSubtries_;
	std::queue<bool> pendingAcks_;
public:
	/**
	 * Creates a new Synchronization Process for the given set. If a
	 * diff handler is passed, it will be used as default for the step
	 * call, which hasn't passed a dedicated callback, or DiffHandler
	 * reference.
	 *
	 * \param set local set, which should be synchronized
	 * \param handler which should be called as default, if no other is given
	 */
	SynchronizationProcess(Set * set, AbstractDiffHandler * handler = NULL);
	virtual ~SynchronizationProcess();
	/**
	 * Calculates the optimal size of a sending buffer for the given network parameter.
	 *
	 * \param RTT round trip time in microseconds
	 * \param bandwidth in bits per second
	 * \return the optimal size of a sending buffer
	 */
	std::size_t
	calcOutputBufferSize(const size_t RTT, const size_t bandwidth) const;
	/**
	 * \return true, if sync is done
	 */
	virtual bool done() const;
	/**
	 * Checks, if the given hash is the same as the local trie root.
	 * If it is, the sync process is done and marked as equal.
	 *
	 * \param hash of the remote trie root
	 * \return true, if hash is equal to local trie root
	 */
	virtual bool isEqual(const unsigned char* hash);
	/**
	 * \return true, while any bloom filter data is available to be sent
	 */
	virtual bool isBloomFilterOutputAvail() const;
	/**
	 * Reads the next bloom filter data and writes them into
	 * the given buffer. The maximum data size is given by length.
	 *
	 * \param buffer where the bf data should be written to
	 * \param length of the buffer
	 * \return the size of the written bloom filter data
	 */
	virtual std::size_t readNextBloomFilterChunk(unsigned char * buffer,
			const std::size_t length);
	/**
	 *
	 */
	virtual void processBloomFilterChunk(const unsigned char * buffer,
			const std::size_t length, AbstractDiffHandler& handler);
	/**
	 * \return true, if the trie has a root and it has been written into hash
	 */
	virtual bool getRootHash(unsigned char * hash);
	/**
	 * \return true, if the trie has a root and it has been written into hash
	 */
	virtual bool getRootHashForSending(unsigned char * hash);
	/**
	 * Returns the next subtrie, which must be sent
	 */
	virtual size_t getSubTrie(unsigned char * buffer, const size_t buffersize);
	/**
	 * Processes the given subtrie and returns the size of the not yet
	 * sent acknowledgment field. This field can be written by calling
	 * readSomeTrieAcks
	 */
	virtual size_t processSubTrie(const unsigned char * buffer,
			const std::size_t length);
	/**
	 *
	 */
	virtual size_t readSomeTrieAcks(unsigned char * buffer,
			const std::size_t length, std::size_t * numberOfAcks);
	/**
	 *
	 */
	virtual void processAcks(const unsigned char * buffer,
			const std::size_t length, const std::size_t numberOfAcks,
			AbstractDiffHandler& handler);
	/**
	 *
	 */
	virtual bool isSubtrieOutputAvailable();
	/**
	 * \return true, until any sent hash hasn't been acked
	 */
	virtual bool isSubtrieUnacked() const;
	/**
	 * \return the size of sent data in bytes
	 */
	virtual std::size_t getSentBytes() const;
	/**
	 * \return the size of any processed data in bytes
	 */
	virtual std::size_t getReceivedBytes() const;
	/**
	 * \return true while any pending ack is available to be sent
	 */
	virtual bool isAckOutputAvailable() const;

	virtual std::size_t getMinTrieBuffer() const;
	virtual std::size_t getMinBFBuffer() const;
	virtual std::size_t getMinBuffer() const;
};

/**
 * Main class to use for a set, which should be synchronized
 * over a network or locally. It provides mechanisms to add, find
 * and remove set members. Members are only cryptographic hash
 * keys or binary data, which is hashed automatically on insertion
 * or find.
 */
class Set {
	friend class SynchronizationProcess;
	friend class SetTest;
private:
	/// maximum number of allowed set elements
	size_t maxSize_;
	/// true if the maximum is a hard limit
	bool hardMaximum_;
	/// The used cryptographic hash function
	const crypto::CryptoHash hash_;
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

	/**
	 * Copies the result of the search for the given key
	 * into a newly allocated memory position and writes the
	 * pointer to value, if the find has been successful.
	 * The size of the allocated memory will be passed to
	 * valueSize.
	 * If no entry has been found, the valueSize is set to 0,
	 * the value pointing is NULL and false is returned.
	 *
	 * \param key to be searched for
	 * \param value pointer to the stored value
	 * \param valueSize of the returned value
	 * \return true on success, otherwise false
	 */
	virtual bool get(const unsigned char * key, unsigned char ** value,
			std::size_t * valueSize);

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
	/**
	 * To synchronize this set with another one, a sync "process" or must
	 * been spawned. The process tracks the state of the sync. It must be
	 * destroyed before this set is been destroyed.
	 *
	 * \return a new synchronization process
	 */
	virtual SynchronizationProcess * createSyncProcess();
	/**
	 * \return true, if both tries are equal
	 */
	virtual bool operator ==(const Set& other) const;
	/**
	 * \return true, if both tries not equal
	 */
	virtual bool operator !=(const Set& other) const;
	/**
	 * \return the used crypto hash function
	 */
	virtual const crypto::CryptoHash& getHashFunction() const;

	/**
	 * \return the actual size of the used bloom filter in bytes
	 */
	virtual std::size_t getBFSize() const;
	/**
	 * \return the number of hashing functions of the uses bloom filter
	 */
	virtual std::size_t getBFFunctionCount() const;
	/**
	 * To create a trie sync data structure, a minimum size for buffer
	 * is needed. So the minimal size can be requested.
	 *
	 * \return buffer size
	 */
	virtual std::size_t getMinSyncTrieBuffer() const;
	/**
	 * To sync parts of the bloom filter, a minimal size is needed
	 * to save a part. This can be requested by calling this method.
	 *
	 * \return buffer size
	 */
	virtual std::size_t getMinSyncBFBuffer() const;
	/**
	 * The minimal buffer for synchronizing sets is given by this
	 * method. It returns the maximum of the minimal buffer size for
	 * triesync and the minimal buffer size for bloom filter sync
	 *
	 * \return buffer size
	 */
	virtual std::size_t getMinSyncBuffer() const;
	/**
	 * All persistent data of this set is saved in this path.
	 *
	 * \return the path to the saved files of the set
	 */
	virtual std::string getPath() const;
	/**
	 * Returns the full trie as a graph in dot language
	 *
	 * \return dot string
	 */
	virtual std::string getTrieToDot() const;
	/**
	 * Returns the full trie as a graph in dot language with the
	 * given prefix at the beginning of each node
	 *
	 * \param prefix for the nodes
	 * \return dot string
	 */
	virtual std::string getTrieToDot(const std::string prefix) const;
};
}

#endif /* SET_HPP_ */
