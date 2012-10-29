/*
 * DiffHandler.h
 *
 *      Author: Till Lorentzen
 */

#ifndef DIFFHANDLER_H_
#define DIFFHANDLER_H_
#include <vector>
#include <ostream>
#include <setsync/set.h>

namespace setsync {


class DiffHandler {
public:
	virtual void
			handle(const unsigned char * hash, const std::size_t hashsize) = 0;
	void operator()(const unsigned char * hash, const std::size_t hashsize);
};

class ListDiffHandler: public DiffHandler {
private:
	std::vector<unsigned char *> list_;
public:
	ListDiffHandler();
	virtual void handle(const unsigned char * hash, const std::size_t hashsize);
	virtual ~ListDiffHandler();
	virtual const unsigned char * operator[](unsigned int const& index) const;
	virtual const std::size_t size();
};

class OutputStreamDiffHandler: public DiffHandler {
private:
	std::ostream& out_;
public:
	OutputStreamDiffHandler(std::ostream& out);
	virtual ~OutputStreamDiffHandler() {
	}
	;
	virtual void handle(const unsigned char * hash, const std::size_t hashsize);
};

class C_DiffHandler: public DiffHandler {
private:
	void *closure_;
	diff_callback * callback_;
public:
	C_DiffHandler(diff_callback *callback, void *closure) :
		closure_(closure), callback_(callback) {
	}
	virtual ~C_DiffHandler() {
	}
	virtual void handle(const unsigned char * hash, const std::size_t hashsize);
};
}

#endif /* DIFFHANDLER_H_ */
