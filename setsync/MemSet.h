
#ifndef MEMSET_HPP_
#define MEMSET_HPP_
#include "Set.hpp"

namespace setsync {


class MemSet : public Set {
public:
	MemSet();
	virtual ~MemSet();
	void clear();
	bool insert(const char * key);
	bool erase(const char * key);
	bool find(const char * key);
};

}

#endif /* MEMSET_HPP_ */
