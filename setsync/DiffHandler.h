/*
 * DiffHandler.h
 *
 *      Author: Till Lorentzen
 */

#ifndef DIFFHANDLER_H_
#define DIFFHANDLER_H_
#include <vector>

namespace setsync {

class DiffHandler {
public:
	virtual void handle(const unsigned char * hash, const std::size_t hashsize) = 0;
	void operator()(const unsigned char * hash, const std::size_t hashsize);
};

class ListDiffHandler : public DiffHandler {
private:
	std::vector<unsigned char *> list_;
public:
	ListDiffHandler();
	virtual void handle(const unsigned char * hash, const std::size_t hashsize);
	virtual ~ListDiffHandler();
	virtual const unsigned char * operator[](unsigned int const& index) const;
	virtual const std::size_t size();
};

class CoutDiffHandler : public DiffHandler {
public:
	virtual void handle(const unsigned char * hash, const std::size_t hashsize);
};
}

#endif /* DIFFHANDLER_H_ */
