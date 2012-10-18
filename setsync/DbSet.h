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

class DbSet : public Set {
private:
	Db * bfdb;
	Db * triedb;
public:
	DbSet(const uint64_t maxSize = 1000, const bool hardMaximum = false,
			const std::size_t hashsize = SHA_DIGEST_LENGTH);
	virtual ~DbSet();
};

}

#endif /* DBSET_H_ */
