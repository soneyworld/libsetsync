/*
 * OutputFunctions.cpp
 *
 *  Created on: 16.10.2012
 *      Author: Till Lorentzen
 */

#include "OutputFunctions.h"
#include <sstream>
#include <iomanip>
#include <setsync/sha1.h>
namespace utils {

std::string OutputFunctions::CryptoHashtoString(
		const unsigned char * hash) {
	return CryptoHashtoString(hash, SHA_DIGEST_LENGTH);
}
std::string OutputFunctions::CryptoHashtoString(
		const unsigned char * hash, const std::size_t hashSize) {
	std::stringstream ss;
	ss << std::hex << std::uppercase;
	for(std::size_t i = 0; i < hashSize; i++){
		unsigned short c = hash[i];
		ss << std::setw(2) << std::setfill('0') << c;
	}
	return ss.str();
}

}
