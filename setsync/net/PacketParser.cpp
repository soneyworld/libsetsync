/*
 * PacketParser.cpp
 *
 *      Author: Till Lorentzen
 */

#include "PacketParser.h"

namespace setsync {

namespace net {

PacketParser::PacketParser(const setsync::Set& set) :
	set_(set) {

}

PacketParser::~PacketParser() {
}

std::size_t PacketParser::processInput(void * inbuf, const std::size_t length,
		AbstractDiffHandler& diffhandler) {
	//TODO
	throw "not yet implemented";
}

void PacketParser::registerInputHandler(const PacketHeader::Type t,
		AbstractInputPacketHandler * handler) {
	this->inputHandler_[t] = handler;
}
void PacketParser::unregisterInputHandler(const PacketHeader::Type t,
		AbstractInputPacketHandler * handler) {
	if (this->inputHandler_[t] == handler) {
		this->inputHandler_.erase(t);
	}
}
void PacketParser::registerOutputHandler(AbstractOutputPacketHandler * handler) {
	this->outputHandler_.push_front(handler);
}
void PacketParser::unregisterOutputHandler(
		AbstractOutputPacketHandler * handler) {
	this->outputHandler_.remove(handler);
}
}

}
