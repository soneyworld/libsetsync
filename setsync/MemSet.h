#ifndef MEMSET_HPP_
#define MEMSET_HPP_
#include "Set.hpp"
#include <setsync/utils/CryptoHash.h>

namespace setsync {

class MemSet: public Set {
public:
	MemSet(const utils::CryptoHash& hash, const uint64_t maxSize = 1000,
			const bool hardMaximum = false);
	virtual ~MemSet();
	virtual SynchronizationStrategy createSyncStrategy(void);
};

}

#endif /* MEMSET_HPP_ */
