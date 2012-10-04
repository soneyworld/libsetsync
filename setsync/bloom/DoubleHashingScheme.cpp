/*
 * DoubleHashingScheme.cpp
 *
 *      Author: Till Lorentzen
 */

#include "DoubleHashingScheme.h"
#include <limits>

namespace bloom {

DoubleHashingScheme::DoubleHashingScheme() {}
DoubleHashingScheme::~DoubleHashingScheme() {}
ExtendedDoubleHashingScheme::ExtendedDoubleHashingScheme() {}
ExtendedDoubleHashingScheme::~ExtendedDoubleHashingScheme() {}

uint64_t DoubleHashingScheme::hash(const unsigned char * input,
		const std::size_t length) const {
	return hash(input, length, 0);
}
uint64_t DoubleHashingScheme::hash(const unsigned char * input,
		const std::size_t length, const std::size_t function) const {
	if(length!=20)
		throw "Must be implemented";
	uint64_t f1 = 0;
	uint64_t f2 = 0;
	for(int i = 0;i< sizeof(uint64_t); i++){
		f1 = f1 | *(input+i);
		f2 = f2 | *(input+i+sizeof(uint64_t));
		f1 = f1 << 8;
		f2 = f2 << 8;
	}
	return f1 + function * f2;
}
size_t DoubleHashingScheme::count() const {
	return std::numeric_limits<size_t>::max();
}

uint64_t ExtendedDoubleHashingScheme::hash(const unsigned char * input,
		const std::size_t length, const std::size_t function) const{
	if(length!=20)
		throw "Must be implemented";
	uint64_t f1 = 0;
	uint64_t f2 = 0;
	for(int i = 0;i< sizeof(uint64_t); i++){
		f1 = f1 | *(input+i);
		f2 = f2 | *(input+i+sizeof(uint64_t));
		f1 = f1 << 8;
		f2 = f2 << 8;
	}
	return f1 + function * f2 + extendedHash(function);
}

uint64_t ExtendedDoubleHashingScheme::extendedHash(const std::size_t i) const{
	//TODO Hash function should be added, otherwise
	return i;
}

}
