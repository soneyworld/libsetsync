/*
 * SpeedTest.h
 *
 *      Author: Till Lorentzen
 */

#ifndef SPEEDTEST_H_
#define SPEEDTEST_H_

#include "Test.h"
#include <setsync/storage/KeyValueStorage.h>
#include <setsync/crypto/CryptoHash.h>

namespace evaluation {
using namespace setsync;
using namespace setsync::storage;

class SpeedTest: public Test {
private:
	AbstractKeyValueStorage& storage_;
	crypto::CryptoHash hash;
public:
	SpeedTest(AbstractKeyValueStorage& storage);
	virtual ~SpeedTest();
	virtual void run();
};

}

#endif /* SPEEDTEST_H_ */
