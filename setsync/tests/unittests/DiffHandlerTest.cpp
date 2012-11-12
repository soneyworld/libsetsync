/*
 * DiffHandlerTest.cpp
 *
 *      Author: Till Lorentzen
 */

#include "DiffHandlerTest.h"
#include <string.h>
#include <setsync/utils/OutputFunctions.h>

namespace setsync {
void callback(void *closure, const unsigned char * hash, const size_t hashsize) {
	utils::CryptoHash h;
	if (hashsize != h.getHashSize())
		throw "";
	unsigned char bla1[h.getHashSize()];
	h(bla1, "bla1");
	DiffHandlerTest * testcase = static_cast<DiffHandlerTest *> (closure);
	testcase->counts++;
}

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
	hash_(hash, "bla1");
	OutputStreamDiffHandler handler(ss);
	handler.handle(hash, hash_.getHashSize());
	std::string s = utils::OutputFunctions::CryptoHashtoString(hash,
			hash_.getHashSize());
	CPPUNIT_ASSERT(s==ss.str());
}

void DiffHandlerTest::testC_Handle() {
	unsigned char hash[hash_.getHashSize()];
	void * closure = this;
	diff_callback * callback_ = &callback;
	C_DiffHandler handler(callback_, closure);
	hash_(hash, "bla1");
	CPPUNIT_ASSERT(counts == 0);
	handler.handle(hash, hash_.getHashSize());
	CPPUNIT_ASSERT(counts == 1);
	handler.handle(hash, hash_.getHashSize());
	CPPUNIT_ASSERT(counts == 2);
	handler.handle(hash, hash_.getHashSize());
	CPPUNIT_ASSERT(counts == 3);
}

void DiffHandlerTest::testFilteredHandle() {
	unsigned char hash[hash_.getHashSize()];
	void * closure = this;
	diff_callback * callback_ = &callback;
	UniqueFilterDiffHandler handler(callback_, closure);
	hash_(hash, "bla1");
	CPPUNIT_ASSERT(counts == 0);
	handler.handle(hash, hash_.getHashSize());
	CPPUNIT_ASSERT(counts == 1);
	handler.handle(hash, hash_.getHashSize());
	CPPUNIT_ASSERT(counts == 1);
	handler.handle(hash, hash_.getHashSize());
	CPPUNIT_ASSERT(counts == 1);
}
}
