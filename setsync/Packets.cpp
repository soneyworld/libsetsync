/*
 * Packets.cpp
 *
 *  Created on: 14.12.2012
 *      Author: Till Lorentzen
 */

#include "Packets.h"
#include <setsync/utils/bitset.h>

namespace setsync {

Packet::Packet(const unsigned char * header) :
	t_(UNDEFINED), size_(0), header_(0) {
	if (!BITTEST(header,0)) {
		if (!BITTEST(header, 1)) {
			t_ = DATA;
		} else {
			t_ = FILTER;
		}
		for (uint8_t i = 0; i < 6; i++) {
			if (BITTEST(header,2+i)) {
				this->t_ = UNDEFINED;
				break;
			}
		}
	} else {
		if (BITTEST(header,1)) {
			this->t_ = GET_REQUEST;
		} else {
			this->t_ = SUBTRIE;
		}
		for (uint8_t i = 0; i < 6; i++) {
			if (BITTEST(header,2+i)) {
				this->size_++;
			}
			this->size_ = this->size_ << 1;
		}
	}
}

Packet::Packet(const Type t, const std::size_t size) :
	t_(t), size_(size) {
	throw "not yet implemented";
}

Packet::~Packet() {
}

void Packet::addHeaderByte(unsigned char * nextHeaderByte) {
	if (this->t_ == DATA) {
		unsigned char nextHead = *nextHeaderByte;

		header_++;
	}
}

bool Packet::isHeaderComplete() {
	if (t_ == DATA) {
		if (this->header_ >= 8) {
			return true;
		} else {
			return false;
		}
	} else if (t_ == UNDEFINED) {
		throw "not yet implemented";
	} else {
		return true;
	}
}

std::size_t Packet::getPacketSize() const {
	return this->size_;
}

Packet::Type Packet::getType() const {
	return this->t_;
}
}
