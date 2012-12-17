/*
 * PacketTest.h
 *
 *      Author: Till Lorentzen
 */

#ifndef PACKETTEST_H_
#define PACKETTEST_H_
#include <cppunit/extensions/HelperMacros.h>
#include <setsync/utils/CryptoHash.h>
#include <setsync/Packets.h>

namespace setsync {

class PacketHeaderTest: public CPPUNIT_NS::TestCase {

CPPUNIT_TEST_SUITE( PacketHeaderTest);
		CPPUNIT_TEST( testConstructor);
		CPPUNIT_TEST( testHeaderSize);
		CPPUNIT_TEST( testParsing);
	CPPUNIT_TEST_SUITE_END();
protected:
	void testConstructor();
	void testHeaderSize();
	void testParsing();
};
CPPUNIT_TEST_SUITE_REGISTRATION(PacketHeaderTest);
}

#endif /* PACKETTEST_H_ */
