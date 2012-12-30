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
		DATA, FILTER, SUBTRIE_REQUEST, SUBTRIE, COMPRESSED_FILTER, UNDEFINED
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
	/**
	 * \return true iff the header has no more pending informations
	 */
	bool isInputHeaderComplete();
	std::size_t getPacketSize() const;
	Type getType() const ;
	static Type getType(const unsigned char* header);
	void addHeaderByte(unsigned char * nextHeaderByte);
	size_t getHeaderSize() const;
	static size_t getHeaderSize(const Type& t);
	void writeHeaderToBuffer(unsigned char * buffer);
};

}

#endif /* PACKETS_H_ */
