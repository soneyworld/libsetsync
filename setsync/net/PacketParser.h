/*
 * PacketParser.h
 *
 *      Author: Till Lorentzen
 */

#ifndef PACKETPARSER_H_
#define PACKETPARSER_H_

#include <setsync/net/Packets.h>
#include <setsync/Set.hpp>
#include <setsync/DiffHandler.h>
#include <list>
#include <map>
#include <queue>

namespace setsync {

namespace net {

class AbstractInputPacketHandler {
public:
	virtual std::size_t processInputPacket(const InPacket& packet, void * inbuf, const std::size_t length,
			AbstractDiffHandler& diffhandler) = 0;
};

class AbstractOutputPacketHandler {
public:
	virtual std::size_t getMinimumPayloadSize() const = 0;
	virtual std::size_t
			writeOutput(void * outbuf, const std::size_t maxlength) = 0;
};

class PacketParser {
private:
	const setsync::Set& set_;
	std::map<PacketHeader::Type, AbstractInputPacketHandler *> inputHandler_;
	std::list<AbstractOutputPacketHandler *> outputHandler_;
	AbstractOutputPacketHandler * currentOutputHandler_;
	AbstractInputPacketHandler * currentInputHandler_;
public:
	PacketParser(const setsync::Set& set);
	virtual ~PacketParser();
	virtual std::size_t processInput(void * inbuf, const std::size_t length,
			AbstractDiffHandler& diffhandler);
	virtual void registerInputHandler(const PacketHeader::Type t,
			AbstractInputPacketHandler * handler);
	virtual void unregisterInputHandler(const PacketHeader::Type t,
			AbstractInputPacketHandler * handler);
	virtual void registerOutputHandler(AbstractOutputPacketHandler * handler);
	virtual void unregisterOutputHandler(AbstractOutputPacketHandler * handler);
};

}

}

#endif /* PACKETPARSER_H_ */
