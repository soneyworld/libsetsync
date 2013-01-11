/*
 * OutputFunctions.h
 *
 *  Created on: 16.10.2012
 *      Author: Till Lorentzen
 */

#ifndef OUTPUTFUNCTIONS_H_
#define OUTPUTFUNCTIONS_H_
#include <string>
namespace setsync {
namespace utils {

class OutputFunctions {
public:
	static std::string
	CryptoHashtoString(const unsigned char * hash);
	static std::string CryptoHashtoString(const unsigned char * hash,
			const std::size_t hashSize);
	static std::string ArrayToBitString(const unsigned char * array,
			const std::size_t bitcount);
};
}
}
#endif /* OUTPUTFUNCTIONS_H_ */
