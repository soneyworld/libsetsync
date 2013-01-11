/*
 * SetTest.h
 *
 *      Author: Till Lorentzen
 */

#ifndef SETTEST_H_
#define SETTEST_H_

#include "Test.h"
#include <setsync/Set.hpp>
#include <setsync/crypto/CryptoHash.h>
#include <setsync/config/Configuration.h>

namespace evaluation {

class SetTest: public Test {
private:
	setsync::Set set_;
	std::string storage_;
public:
	SetTest(const setsync::config::Configuration& c);
	virtual ~SetTest();
	virtual void run();
	virtual void insert();
};

}

#endif /* SETTEST_H_ */
