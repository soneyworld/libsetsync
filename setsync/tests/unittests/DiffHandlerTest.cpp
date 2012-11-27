/*
 * DiffHandlerTest.cpp
 *
 *      Author: Till Lorentzen
 */

#include "DiffHandlerTest.h"
#include <string.h>
#include <setsync/utils/OutputFunctions.h>

namespace setsync {
void callback(void *closure, const unsigned char * hash, const size_t hashsize,
		const size_t existsLocally) {
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
	handler.handle(hash, hash_.getHashSize(), true);
	CPPUNIT_ASSERT(handler.size() == 1);
	CPPUNIT_ASSERT(memcmp(hash,handler[0].first,hash_.getHashSize())==0);
	CPPUNIT_ASSERT(handler[0].second == true);
	hash_(hash, "bla2");
	handler.handle(hash, hash_.getHashSize(), false);
	CPPUNIT_ASSERT(handler.size() == 2);
	handler.handle(hash, hash_.getHashSize(), false);
	CPPUNIT_ASSERT(handler.size() == 2);
	CPPUNIT_ASSERT(memcmp(hash,handler[1].first,hash_.getHashSize())==0);
	CPPUNIT_ASSERT(handler[1].second == false);
	handler.clear();
	CPPUNIT_ASSERT(handler.size() == 0);
}

void DiffHandlerTest::testOutputStreamHandle() {
	std::stringstream ss;
	unsigned char hash[hash_.getHashSize()];
	hash_(hash, "bla1");
	OutputStreamDiffHandler handler(ss);
	handler.handle(hash, hash_.getHashSize(), true);
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
	handler.handle(hash, hash_.getHashSize(), true);
	CPPUNIT_ASSERT(counts == 1);
	handler.handle(hash, hash_.getHashSize(), true);
	CPPUNIT_ASSERT(counts == 2);
	handler.handle(hash, hash_.getHashSize(), true);
	CPPUNIT_ASSERT(counts == 3);
}

void DiffHandlerTest::testFilteredHandle() {
	unsigned char hash[hash_.getHashSize()];
	void * closure = this;
	diff_callback * callback_ = &callback;
	UniqueFilterDiffHandler handler(callback_, closure);
	hash_(hash, "bla1");
	CPPUNIT_ASSERT(counts == 0);
	handler.handle(hash, hash_.getHashSize(), true);
	CPPUNIT_ASSERT(counts == 1);
	handler.handle(hash, hash_.getHashSize(), true);
	CPPUNIT_ASSERT(counts == 1);
	handler.handle(hash, hash_.getHashSize(), true);
	CPPUNIT_ASSERT(counts == 1);
}
}
