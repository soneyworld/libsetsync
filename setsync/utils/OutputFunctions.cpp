/*
 * OutputFunctions.cpp
 *
 *  Created on: 16.10.2012
 *      Author: Till Lorentzen
 */

#include "OutputFunctions.h"
#include <sstream>
#include <iomanip>
#include <setsync/utils/bitset.h>
#include <setsync/crypto/CryptoHash.h>
namespace setsync {
namespace utils {

std::string OutputFunctions::CryptoHashtoString(const unsigned char * hash) {
	return CryptoHashtoString(hash, CryptoHash::getDefaultDigitLength());
}
std::string OutputFunctions::CryptoHashtoString(const unsigned char * hash,
		const std::size_t hashSize) {
	std::stringstream ss;
	ss << std::hex << std::uppercase;
	for (std::size_t i = 0; i < hashSize; i++) {
		unsigned short c = hash[i];
		ss << std::setw(2) << std::setfill('0') << c;
	}
	return ss.str();
}
std::string OutputFunctions::ArrayToBitString(const unsigned char * array,
		const std::size_t bitcount) {
	std::stringstream ss;
	for (std::size_t i = 0; i < bitcount; i++) {
		if (BITTEST(array,i)) {
			ss << "1";
		} else {
			ss << "0";
		}
	}
	return ss.str();
}
}
}
