/*
 * FSBloomFilter.cpp
 *
 *      Author: Till Lorentzen
 */

#include "FSBloomFilter.h"

namespace bloom {

FSBloomFilter::FSBloomFilter(const uint64_t maxNumberOfElements,
		const bool hardMaximum, const float falsePositiveRate,
		const std::size_t hashsize) {

}

FSBloomFilter::~FSBloomFilter() {
}

void FSBloomFilter::load(std::istream &in, const uint64_t numberOfElements){

}
uint64_t FSBloomFilter::save(std::ostream &out){

}
void FSBloomFilter::clear(){

}
}
