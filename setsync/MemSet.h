#ifndef MEMSET_HPP_
#define MEMSET_HPP_
#include "Set.hpp"

namespace setsync {

class MemSet: public Set {
public:
	MemSet(const uint64_t maxSize = 1000, const bool hardMaximum = false,
			const std::size_t hashsize = SHA_DIGEST_LENGTH);
	virtual ~MemSet();
};

}

#endif /* MEMSET_HPP_ */
