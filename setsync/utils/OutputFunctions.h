/*
 * OutputFunctions.h
 *
 *  Created on: 16.10.2012
 *      Author: Till Lorentzen
 */

#ifndef OUTPUTFUNCTIONS_H_
#define OUTPUTFUNCTIONS_H_
#include <string>
namespace utils {

class OutputFunctions {
public:
	static std::string
			CryptoHashtoString(const unsigned char * hash);
	static std::string CryptoHashtoString(const unsigned char * hash,
			std::size_t hashSize);
};
}

#endif /* OUTPUTFUNCTIONS_H_ */
