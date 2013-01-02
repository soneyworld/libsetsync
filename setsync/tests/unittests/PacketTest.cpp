/*
 * PacketTest.cpp
 *
 *      Author: Till Lorentzen
 */

#include "PacketTest.h"
#include <string.h>

namespace setsync {

namespace net {
void PacketHeaderTest::testConstructor() {
	PacketHeader header(PacketHeader::DATA, 40);
	CPPUNIT_ASSERT(header.getType() == PacketHeader::DATA);
	CPPUNIT_ASSERT(header.getPacketSize() == 40);
}

void PacketHeaderTest::testHeaderSize() {
	PacketHeader bfheader(PacketHeader::FILTER);
	PacketHeader dataheader(PacketHeader::DATA);
	PacketHeader subtrieheader(PacketHeader::SUBTRIE);
	PacketHeader requestheader(PacketHeader::SUBTRIE_REQUEST);
	PacketHeader compbfheader(PacketHeader::COMPRESSED_FILTER);
	PacketHeader errheader(PacketHeader::ERROR_MSG);
	PacketHeader rootheader(PacketHeader::TRIE_ROOT);
	CPPUNIT_ASSERT(bfheader.getHeaderSize() == 9);
	CPPUNIT_ASSERT(dataheader.getHeaderSize() == 9);
	CPPUNIT_ASSERT(subtrieheader.getHeaderSize() == 1);
	CPPUNIT_ASSERT(requestheader.getHeaderSize() == 1);
	CPPUNIT_ASSERT(compbfheader.getHeaderSize() == 9);
	CPPUNIT_ASSERT(errheader.getHeaderSize() == 9);
	CPPUNIT_ASSERT(rootheader.getHeaderSize() == 9);
}

void PacketHeaderTest::testParsing() {
	std::string errormsg = "unknown packet found";
	unsigned char buffer[9];
	int pos;
	PacketHeader outbfheader(PacketHeader::FILTER, 1024);
	PacketHeader outdataheader(PacketHeader::DATA, 2048);
	PacketHeader outsubtrieheader(PacketHeader::SUBTRIE, 1);
	PacketHeader outrequestheader(PacketHeader::SUBTRIE_REQUEST, 34);
	PacketHeader outcompbfheader(PacketHeader::COMPRESSED_FILTER, 34);
	PacketHeader errheader(PacketHeader::ERROR_MSG,errormsg.size());
	PacketHeader rootheader(PacketHeader::TRIE_ROOT,20);
	memset(buffer, 0, 9);
	outbfheader.writeHeaderToBuffer(buffer);
	pos = 0;
	PacketHeader inbfheader(buffer);
	while (!inbfheader.isInputHeaderComplete()) {
		pos++;
		inbfheader.addHeaderByte(buffer + pos);
	}
	CPPUNIT_ASSERT(inbfheader.getType() == outbfheader.getType());
	CPPUNIT_ASSERT(inbfheader.getHeaderSize() == outbfheader.getHeaderSize());
	CPPUNIT_ASSERT(inbfheader.getPacketSize() == outbfheader.getPacketSize());
	memset(buffer, 0, 9);
	outdataheader.writeHeaderToBuffer(buffer);
	pos = 0;
	PacketHeader indataheader(buffer);
	while (!indataheader.isInputHeaderComplete()) {
		pos++;
		indataheader.addHeaderByte(buffer + pos);
	}
	CPPUNIT_ASSERT(indataheader.getType() == outdataheader.getType());
	CPPUNIT_ASSERT(indataheader.getHeaderSize() == outdataheader.getHeaderSize());
	CPPUNIT_ASSERT(indataheader.getPacketSize() == outdataheader.getPacketSize());
	memset(buffer, 0, 9);
	outsubtrieheader.writeHeaderToBuffer(buffer);
	pos = 0;
	PacketHeader insubtrieheader(buffer);
	while (!insubtrieheader.isInputHeaderComplete()) {
		pos++;
		insubtrieheader.addHeaderByte(buffer + pos);
	}
	CPPUNIT_ASSERT(insubtrieheader.getType() == outsubtrieheader.getType());
	CPPUNIT_ASSERT(insubtrieheader.getHeaderSize() == outsubtrieheader.getHeaderSize());
	CPPUNIT_ASSERT(insubtrieheader.getPacketSize() == outsubtrieheader.getPacketSize());
	memset(buffer, 0, 9);
	outrequestheader.writeHeaderToBuffer(buffer);
	pos = 0;
	PacketHeader inrequestheader(buffer);
	while (!inrequestheader.isInputHeaderComplete()) {
		pos++;
		inrequestheader.addHeaderByte(buffer + pos);
	}
	CPPUNIT_ASSERT(inrequestheader.getType() == outrequestheader.getType());
	CPPUNIT_ASSERT(inrequestheader.getHeaderSize() == outrequestheader.getHeaderSize());
	CPPUNIT_ASSERT(inrequestheader.getPacketSize() == outrequestheader.getPacketSize());
	memset(buffer, 0, 9);
	outcompbfheader.writeHeaderToBuffer(buffer);
	pos = 0;
	PacketHeader incompbfheader(buffer);
	while (!incompbfheader.isInputHeaderComplete()) {
		pos++;
		incompbfheader.addHeaderByte(buffer + pos);
	}
	CPPUNIT_ASSERT(incompbfheader.getType() == outcompbfheader.getType());
	CPPUNIT_ASSERT(incompbfheader.getHeaderSize() == outcompbfheader.getHeaderSize());
	CPPUNIT_ASSERT(incompbfheader.getPacketSize() == outcompbfheader.getPacketSize());

}
}
}
