/*
 * DoubleHashingScheme.cpp
 *
 *      Author: Till Lorentzen
 */

#include "DoubleHashingScheme.h"
#include <limits>
#include <string.h>

namespace bloom {

DoubleHashingScheme::DoubleHashingScheme(const std::size_t hashsize) :
	hashsize_(hashsize) {
}
DoubleHashingScheme::~DoubleHashingScheme() {
}
ExtendedDoubleHashingScheme::ExtendedDoubleHashingScheme(
		const std::size_t hashsize) :
	DoubleHashingScheme(hashsize) {
}
ExtendedDoubleHashingScheme::~ExtendedDoubleHashingScheme() {
}

uint64_t DoubleHashingScheme::hash(const unsigned char * input,
		const std::size_t length) const {
	return hash(input, length, 0);
}
uint64_t DoubleHashingScheme::hash(const unsigned char * input,
		const std::size_t length, const std::size_t function) const {
	if (hashsize_ < sizeof(uint64_t) * 2 || this->hashsize_ != length)
		throw "Must be implemented";
	uint64_t f1;
	uint64_t f2;
	memcpy(&f1, input, sizeof(uint64_t));
	memcpy(&f2, input + sizeof(uint64_t), sizeof(uint64_t));
	return f1 + (function + 1) * f2;
}
size_t DoubleHashingScheme::count() const {
	return std::numeric_limits<size_t>::max();
}

uint64_t ExtendedDoubleHashingScheme::hash(const unsigned char * input,
		const std::size_t length, const std::size_t function) const {
	return DoubleHashingScheme::hash(input, length, function) + extendedHash(
			input + 2 * sizeof(uint64_t), length - 2 * sizeof(uint64_t),
			function);
}

uint64_t ExtendedDoubleHashingScheme::extendedHash(const unsigned char * input,
		const std::size_t remaining, const std::size_t function) const {
	//Use third part of hash as hash function 3, otherwise, simply return function
	if (remaining > 0) {
		uint64_t f3 = 0;
		for (uint64_t j = 0; j < sizeof(uint64_t) && j < remaining; j++) {
			f3 = f3 | *(input + j);
			f3 = f3 << 8;
		}
		return f3 * function;
	} else {
		return function;
	}
}

}
