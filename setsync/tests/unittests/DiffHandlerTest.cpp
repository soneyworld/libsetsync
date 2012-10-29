/*
 * DiffHandlerTest.cpp
 *
 *      Author: Till Lorentzen
 */

#include "DiffHandlerTest.h"
#include <setsync/sha1.h>
#include <string.h>
#include <setsync/utils/OutputFunctions.h>

namespace setsync {

void DiffHandlerTest::testListHandle() {
	unsigned char hash[SHA_DIGEST_LENGTH];
	ListDiffHandler handler;
	CPPUNIT_ASSERT(handler.size() == 0);
	SHA1((const unsigned char*) "bla1", 4, hash);
	handler.handle(hash, SHA_DIGEST_LENGTH);
	CPPUNIT_ASSERT(handler.size() == 1);
	CPPUNIT_ASSERT(memcmp(hash,handler[0],SHA_DIGEST_LENGTH)==0);
	SHA1((const unsigned char*) "bla2", 4, hash);
	handler.handle(hash, SHA_DIGEST_LENGTH);
	CPPUNIT_ASSERT(handler.size() == 2);
	handler.handle(hash, SHA_DIGEST_LENGTH);
	CPPUNIT_ASSERT(handler.size() == 2);
	CPPUNIT_ASSERT(memcmp(hash,handler[1],SHA_DIGEST_LENGTH)==0);
}

void DiffHandlerTest::testOutputStreamHandle() {
	std::stringstream ss;
	unsigned char hash[SHA_DIGEST_LENGTH];
	SHA1((const unsigned char*) "bla1", 4, hash);
	OutputStreamDiffHandler handler(ss);
	handler.handle(hash, SHA_DIGEST_LENGTH);
	std::string s = utils::OutputFunctions::CryptoHashtoString(hash,
			SHA_DIGEST_LENGTH);
	CPPUNIT_ASSERT(s==ss.str());
}

}
