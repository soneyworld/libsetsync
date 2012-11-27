/*
 * DbSet.h
 *
 *      Author: Till Lorentzen
 */

#ifndef DBSET_H_
#define DBSET_H_
#include <setsync/Set.hpp>
#include <db_cxx.h>
namespace setsync {
/**
 * This Set is based on Berkeley DB components only. The used bloom filter
 * is the bloom::DBBloomFilter and the trie is the trie::DBTrie.
 */
class DbSet: public Set {
private:
	/// If a temporary folder should be created, this is the name of the folder
	char * db_temp_home;
	/// The Berkeley DB environment used by bloom filter and trie
	DbEnv * env_;
	/// DB pointer for the bloom filter
	Db * bfdb;
	/// DB pointer for the trie
	Db * triedb;
	/**
	 * Removes the given directory with all its containing files and
	 * directories recursively.
	 *
	 * \param path to be removed recursively
	 * \return 0 on success
	 */
	int removeTempDir(const char *path);
public:
	/**
	 * Constructs a berkeley DB based set. If db_home is NULL, a temporary
	 * directory will be created and used until the Object will be destroyed.
	 * Otherwise if db_home is a path, it will be used to save the set in this
	 * path. So it can be used to save an instance and reload its contents the
	 * next time, using a set with the same path.
	 *
	 * \param db_home path where the db should be saved or NULL
	 * \param maxSize for the bloom filter
	 * \param hardMaximum if true, this set will only grow until maxSize is reached
	 * \param falsePositiveRate is a float between 0 and 1 and will be given to bloom filter
	 * \param hashsize of the crypto hash key size in bytes
	 */
	DbSet(const utils::CryptoHash& hash, const char * db_home = NULL,
			const uint64_t maxSize = 1000, const bool hardMaximum = false,
			const float falsePositiveRate = 0.01);
	/**
	 *
	 */
	//TODO Documentation
	virtual SynchronizationStrategy createSyncStrategy(void);
	/**
	 * Destroys the set and if db_home has been NULL, the temporary created folder
	 * will be removed. Otherwise db_home will stay untouched.
	 */
	virtual ~DbSet();
};

}

#endif /* DBSET_H_ */
