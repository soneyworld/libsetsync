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
#include <string>
#include <setsync/utils/CryptoHash.h>
namespace bloom {

class HashFunction {
protected:
	virtual uint64_t hash(const unsigned char * input,
			const std::size_t length, const std::size_t function) const = 0;
public:
	uint64_t operator()(const unsigned char * input,
			const std::size_t length) const;
	uint64_t operator()(const unsigned char * input,
			const std::size_t length, const std::size_t function) const;
	virtual size_t count() const = 0;
};

class HashFunctionFactory {
private:
	HashFunctionFactory() {
	}
	HashFunctionFactory(const HashFunctionFactory&) {
	}
	HashFunctionFactory& operator=(const HashFunctionFactory&) {
		return *this;
	}
	~HashFunctionFactory() {
	}
public:
	static HashFunctionFactory& getInstance();
	HashFunction * createHashFunction(std::string function);
};

class SplittingMDHashFunction: public HashFunction {
protected:
	virtual uint64_t hash(const unsigned char * input,
			const std::size_t length, const std::size_t function) const;
public:
	SplittingMDHashFunction(const std::size_t outputLength,
			const utils::CryptoHash& hash);
	virtual ~SplittingMDHashFunction();
	virtual size_t count() const;
private:
	const utils::CryptoHash& hash_;
	std::size_t outputLength_;
};

class SaltedHashFunction: public HashFunction {
protected:
	virtual uint64_t hash(const unsigned char * input,
			const std::size_t length, const std::size_t function) const;
public:
	SaltedHashFunction(size_t salt_count);
	virtual ~SaltedHashFunction();
	size_t count() const;
private:
	void generate_salt();
	std::vector<int> _salt;
	std::size_t salt_count_;
};

}

#endif /* HASHFUNCTION_H_ */
