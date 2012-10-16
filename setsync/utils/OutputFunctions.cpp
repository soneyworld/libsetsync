/*
 * OutputFunctions.cpp
 *
 *  Created on: 16.10.2012
 *      Author: Till Lorentzen
 */

#include "OutputFunctions.h"
#include <sstream>
#include <setsync/sha1.h>
namespace utils {

std::string OutputFunctions::CryptoHashtoString(
		const unsigned char * hash) {
	return CryptoHashtoString(hash, SHA_DIGEST_LENGTH);
}
std::string OutputFunctions::CryptoHashtoString(
		const unsigned char * hash, const std::size_t hashSize) {
	std::stringstream ss;
	for(std::size_t i = 0; i < hashSize; i++){
		unsigned char c = hash[i];
		ss << std::hex << c;
	}
	return ss.str();
}

}
