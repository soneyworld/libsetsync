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

class Packet {
public:
	enum Type {
		DATA, FILTER, GET_REQUEST, SUBTRIE, UNDEFINED
	};
private:
	enum Type t_;
	std::size_t size_;
	uint8_t header_;
public:
	// Incoming Packet
	Packet(const unsigned char * header);
	// Creating header for outgoing packet
	Packet(const Type t, const std::size_t size = 0);
	virtual ~Packet();
	bool isHeaderComplete();
	std::size_t getPacketSize() const;
	Type getType() const ;
	void addHeaderByte(unsigned char * nextHeaderByte);
};

}

#endif /* PACKETS_H_ */
