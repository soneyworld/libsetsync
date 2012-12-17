/*
 * Packets.h
 *
 *  Created on: 14.12.2012
 *      Author: till
 */

#ifndef PACKETS_H_
#define PACKETS_H_
#include <cstddef>
#include <inttypes.h>

namespace setsync {

class PacketHeader {
public:
	enum Type {
		DATA, FILTER, SUBTRIE_REQUEST, SUBTRIE, UNDEFINED
	};
private:
	enum Type t_;
	uint64_t size_;
	uint8_t inHeaderPos_;
public:
	// Incoming Packet
	PacketHeader(const unsigned char * header);
	// Creating header for outgoing packet
	PacketHeader(const Type t, const std::size_t size = 0);
	virtual ~PacketHeader();
	bool isInputHeaderComplete();
	std::size_t getPacketSize() const;
	Type getType() const ;
	void addHeaderByte(unsigned char * nextHeaderByte);
	size_t getHeaderSize();
	void writeHeaderToBuffer(unsigned char * buffer);
};

}

#endif /* PACKETS_H_ */
