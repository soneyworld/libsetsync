/*
 * PacketParser.cpp
 *
 *      Author: Till Lorentzen
 */

#include "PacketParser.h"

namespace setsync {

namespace net {

PacketParser::PacketParser(const setsync::Set& set) : set_(set) {

}

PacketParser::~PacketParser() {
}

std::size_t PacketParser::processInput(void * inbuf, const std::size_t length,
		AbstractDiffHandler& diffhandler){
	//TODO
	throw "not yet implemented";
}

void PacketParser::registerInputHandler(const PacketHeader::Type t,
		AbstractInputPacketHandler& handler){
	//TODO
	throw "not yet implemented";
}
void PacketParser::unregisterInputHandler(const PacketHeader::Type t,
		AbstractInputPacketHandler& handler){
	//TODO
	throw "not yet implemented";
}
void PacketParser::registerOutputHandler(AbstractOutputPacketHandler& handler){
	//TODO
	throw "not yet implemented";
}
void PacketParser::unregisterOutputHandler(AbstractOutputPacketHandler& handler){
	//TODO
	throw "not yet implemented";
}
}

}
