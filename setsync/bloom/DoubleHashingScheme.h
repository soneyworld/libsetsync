/*
 * DoubleHashingScheme.h
 *
 *      Author: Till Lorentzen
 */

#ifndef DOUBLEHASHINGSCHEME_H_
#define DOUBLEHASHINGSCHEME_H_

#include "HashFunction.h"

namespace bloom {

class DoubleHashingScheme: public HashFunction {
public:
	DoubleHashingScheme();
	virtual ~DoubleHashingScheme();
	virtual uint64_t
	hash(const unsigned char * input, const std::size_t length) const;
	virtual uint64_t hash(const unsigned char * input,
			const std::size_t length, const std::size_t function) const;
	virtual std::size_t count() const;
};

class ExtendedDoubleHashingScheme: public DoubleHashingScheme {
public:
	ExtendedDoubleHashingScheme();
	virtual ~ExtendedDoubleHashingScheme();
	virtual uint64_t hash(const unsigned char * input,
			const std::size_t length, const std::size_t function) const;
private:
	uint64_t extendedHash(const std::size_t i) const;
};
}

#endif /* DOUBLEHASHINGSCHEME_H_ */
