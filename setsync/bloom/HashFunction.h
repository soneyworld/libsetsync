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
#include <setsync/crypto/CryptoHash.h>

namespace setsync {

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
}
}

#endif /* HASHFUNCTION_H_ */
