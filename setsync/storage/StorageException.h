/*
 * StorageException.h
 *
 *      Author: Till Lorentzen
 */

#ifndef STORAGEEXCEPTION_H_
#define STORAGEEXCEPTION_H_

#include <exception>
#include <string>

namespace setsync {

namespace storage {

class StorageException: public std::exception {
private:
	std::string what_;
public:
	StorageException(const char * what) throw () {
		what_ = what;
	}
	StorageException(const std::string& what) throw () {
		what_ = what;
	}
	virtual ~StorageException() throw () {

	}
	/**
	 * \return what problem exists
	 */
	virtual const char* what() {
		return what_.c_str();
	}
};

}

}

#endif /* STORAGEEXCEPTION_H_ */
