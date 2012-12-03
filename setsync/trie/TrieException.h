/*
 * TrieException.h
 *
 *      Author: Till Lorentzen
 */

#ifndef TRIEEXCEPTION_H_
#define TRIEEXCEPTION_H_

namespace trie {
/**
 * This exception should be thrown, if any consistency failure happens
 */
class TrieException: public std::exception {
private:
	/// Message returned by calling what()
	std::string what_;
public:
	TrieException(const char * message) throw () {
		what_ = std::string(message);
	}
	TrieException(const std::string& message) throw () {
		what_ = std::string(message);
	}
	virtual ~TrieException() throw () {

	}
	/**
	 * \return what problem exists
	 */
	virtual const char* what() {
		return what_.c_str();
	}

};
/**
 * This exception should be thrown, if a consistency failure happens
 */
class DbTrieException: public TrieException {
public:
	DbTrieException(const char * message) :
		TrieException(message) {
	}
	DbTrieException(const std::string& message) :
		TrieException(message) {
	}
};
/**
 * This exception should be thrown, if the a node of the trie
 * hasn't been found
 */
class TrieNodeNotFoundException: public TrieException {
public:
	TrieNodeNotFoundException() throw () :
		TrieException("Node not found") {
	}
	virtual ~TrieNodeNotFoundException() throw () {
	}
};
/**
 * This exception should be thrown, if the root node of the trie
 * hasn't been found
 */
class TrieRootNotFoundException: public TrieException {
public:
	TrieRootNotFoundException() throw () :
		TrieException("No root found") {
	}
	virtual ~TrieRootNotFoundException() throw () {
	}
};

}

#endif /* TRIEEXCEPTION_H_ */
