/*
 * HashFunction.cpp
 *
 *      Author: Till Lorentzen
 */

#include "HashFunction.h"
namespace setsync {
namespace bloom {

uint64_t HashFunction::operator ()(const unsigned char * input,
		const std::size_t length) const {
	return this->hash(input, length, 0);
}

uint64_t HashFunction::operator ()(const unsigned char * input,
		const std::size_t length, const std::size_t function) const {
	return this->hash(input, length, function);
}
}
}
