/*
 * HashFunction.h
 *
 *      Author: Till Lorentzen
 */

#ifndef HASHFUNCTION_H_
#define HASHFUNCTION_H_
#include <cstddef>
#include <stdint.h>
#include <vector>

namespace bloom {

class HashFunction {
public:
	virtual uint64_t
	hash(const unsigned char * input, const std::size_t length) const = 0;
	virtual uint64_t hash(const unsigned char * input,
			const std::size_t length, const std::size_t function) const = 0;
	virtual size_t count() const = 0;
};

class SplittingMDHashFunction: public HashFunction {
public:
	SplittingMDHashFunction(const std::size_t outputLength,
			const std::size_t hashSize = 20);
	virtual ~SplittingMDHashFunction();
	virtual size_t count() const;
	virtual uint64_t
	hash(const unsigned char * input, const std::size_t length) const;
	virtual uint64_t hash(const unsigned char * input,
			const std::size_t length, const std::size_t function) const;
private:
	std::size_t hashSize_;
	std::size_t outputLength_;
};

class SaltedHashFunction: public HashFunction {
public:
	SaltedHashFunction(size_t salt_count);
	virtual ~SaltedHashFunction();
	size_t count() const;
	virtual uint64_t
	hash(const unsigned char * input, const std::size_t length) const;
	virtual uint64_t hash(const unsigned char * input,
			const std::size_t length, const std::size_t function) const;
private:
	void generate_salt();
	std::vector<int> _salt;
	std::size_t salt_count_;
};

}

#endif /* HASHFUNCTION_H_ */
