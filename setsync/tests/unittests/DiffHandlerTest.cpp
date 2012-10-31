/*
 * DiffHandlerTest.cpp
 *
 *      Author: Till Lorentzen
 */

#include "DiffHandlerTest.h"
#include <string.h>
#include <setsync/utils/OutputFunctions.h>

namespace setsync {

void DiffHandlerTest::testListHandle() {
	unsigned char hash[hash_.getHashSize()];
	ListDiffHandler handler;
	CPPUNIT_ASSERT(handler.size() == 0);
	hash_(hash, "bla1");
	handler.handle(hash, hash_.getHashSize());
	CPPUNIT_ASSERT(handler.size() == 1);
	CPPUNIT_ASSERT(memcmp(hash,handler[0],hash_.getHashSize())==0);
	hash_(hash, "bla2");
	handler.handle(hash, hash_.getHashSize());
	CPPUNIT_ASSERT(handler.size() == 2);
	handler.handle(hash, hash_.getHashSize());
	CPPUNIT_ASSERT(handler.size() == 2);
	CPPUNIT_ASSERT(memcmp(hash,handler[1],hash_.getHashSize())==0);
}

void DiffHandlerTest::testOutputStreamHandle() {
	std::stringstream ss;
	unsigned char hash[hash_.getHashSize()];
	hash_(hash,"bla1");
	OutputStreamDiffHandler handler(ss);
	handler.handle(hash, hash_.getHashSize());
	std::string s = utils::OutputFunctions::CryptoHashtoString(hash,
			hash_.getHashSize());
	CPPUNIT_ASSERT(s==ss.str());
}

}
