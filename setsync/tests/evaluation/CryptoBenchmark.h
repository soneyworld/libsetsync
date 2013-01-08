/*
 * CryptoBenchmark.h
 *
 *  Created on: 08.01.2013
 *      Author: till
 */

#ifndef CRYPTOBENCHMARK_H_
#define CRYPTOBENCHMARK_H_
#include "Test.h"
#include "StopWatch.h"
#include <setsync/utils/CryptoHash.h>

namespace evaluation {

class CryptoBenchmark: public Test {
public:
	CryptoBenchmark();
	virtual ~CryptoBenchmark();
	virtual void run();
};

}

#endif /* CRYPTOBENCHMARK_H_ */
