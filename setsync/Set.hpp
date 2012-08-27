/*
 * Set.hpp
 *
 *  Created on: 12.07.2012
 *      Author: Till Lorentzen
 */

#ifndef SET_HPP_
#define SET_HPP_
namespace setsync {

class Set {
private:
	size_t size;
	size_t maxSize;
protected:
	void setSize(const size_t s);
	void setMaximumSize(const size_t s);
public:
	Set();
	bool isEmpty() const;
	size_t getSize() const;
	size_t getMaximumSize() const;

	// Lookup
	virtual bool find(const char * key) = 0;

	// Modifiers
	virtual bool insert( const char * key)  = 0;
	virtual bool erase( const char * key) = 0;
	virtual void clear() = 0;

	virtual ~Set();

};

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

#endif /* SET_HPP_ */
