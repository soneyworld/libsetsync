/*
 * Packets.cpp
 *
 *      Author: Till Lorentzen
 */

#include "Packets.h"
#include <setsync/utils/bitset.h>
#include <string.h>
namespace setsync {

namespace net {

PacketHeader::PacketHeader(const unsigned char * header) :
	t_(UNDEFINED), size_(0), inHeaderPos_(0) {
	if (!BITTEST(header,0)) {
		if (!BITTEST(header, 1)) {
			t_ = DATA;
		} else {
			if (BITTEST(header, 2)) {
				t_ = COMPRESSED_FILTER;
			} else {
				t_ = FILTER;
			}
		}
		for (uint8_t i = 0; i < 6; i++) {
			if (BITTEST(header,2+i)) {
				if (i == 0 && (t_ == FILTER || t_ == COMPRESSED_FILTER)) {
					continue;
				} else {
					this->t_ = UNDEFINED;
					break;
				}
			}
		}
	} else {
		if (BITTEST(header,1)) {
			this->t_ = SUBTRIE_REQUEST;
		} else {
			this->t_ = SUBTRIE;
		}
		for (uint8_t i = 0; i < 6; i++) {
			this->size_ = this->size_ << 1;
			if (BITTEST(header,2+i)) {
				this->size_++;
			}
		}
		this->size_++;
	}
}

PacketHeader::PacketHeader(const Type t, const std::size_t size) :
	t_(t), size_(size) {
	if (t_ == SUBTRIE || t_ == SUBTRIE_REQUEST || t_ == TRIE_ROOT) {
		if (size_ > 64) {
			size_ = 64;
			throw "maximum size of this packet type is 64";
		} else if (size_ == 0) {
			size_ = 1;
		}
	}
}

PacketHeader::~PacketHeader() {
}

void PacketHeader::addHeaderByte(unsigned char * nextHeaderByte) {
	if (this->t_ == DATA || t_ == FILTER || t_ == COMPRESSED_FILTER) {
		for (uint8_t i = 0; i < 8; i++) {
			if (BITTEST(nextHeaderByte,i)) {
				this->size_ += 1 << ((inHeaderPos_ * 8) + i);
			}
		}
		inHeaderPos_++;
	} else {
		throw "No more bytes needed!";
	}
}

size_t PacketHeader::getHeaderSize() const {
	return getHeaderSize(t_);
}

PacketHeader::Type PacketHeader::getType(const unsigned char* header) {
	Type t_ = UNDEFINED;
	// first bit is 0 => DATA, ERROR_MSG, FILTER, COMPRESSED_FILTER, TRIE_ROOT
	if (!BITTEST(header,0)) {
		// second bit is 0 => DATA, ERROR_MSG, TRIE_ROOT
		if (!BITTEST(header, 1)) {
			// third bit is 0 => DATA
			if (!BITTEST(header,2)) {
				t_ = DATA;
			} else {
				// forth bit is 0 => ERROR_MSG
				if (!BITTEST(header,4)) {
					t_ = ERROR_MSG;
				} else {
					t_ = TRIE_ROOT;
				}
			}
		}
		//second bit is 1 => FILTER or COMPRESSED_FILTER
		else {
			if (BITTEST(header, 2)) {
				t_ = COMPRESSED_FILTER;
			} else {
				t_ = FILTER;
			}
		}
	} else {
		if (BITTEST(header,1)) {
			t_ = SUBTRIE_REQUEST;
		} else {
			t_ = SUBTRIE;
		}
	}
	return t_;
}

size_t PacketHeader::getHeaderSize(const Type& t) {
	switch (t) {
	case FILTER:
	case COMPRESSED_FILTER:
	case ERROR_MSG:
	case TRIE_ROOT:
	case DATA:
		return sizeof(uint64_t) + sizeof(uint8_t);
	case SUBTRIE:
	case SUBTRIE_REQUEST:
		return sizeof(uint8_t);
	default:
		throw "Unsupported";
	}
}

void PacketHeader::writeHeaderToBuffer(unsigned char * buffer) {
	*buffer = 0;
	switch (t_) {
	case COMPRESSED_FILTER:
		BITSET(buffer,2);
	case FILTER:
		BITSET(buffer,1);
		break;
	case DATA:
		break;
	case ERROR_MSG:
		BITSET(buffer,1);
		break;
	case TRIE_ROOT:
		break;
	case SUBTRIE_REQUEST:
		BITSET(buffer,1);
	case SUBTRIE:
		BITSET(buffer,0);
		break;
	default:
		throw "Unsupported";
	}
	switch (t_) {
	case FILTER:
	case COMPRESSED_FILTER:
	case ERROR_MSG:
	case TRIE_ROOT:
	case DATA: {
		uint64_t s = size_;
		for (int i = 0; i < 64; i++) {
			if (s % 2 == 1) {
				BITSET(buffer+1, i);
			}
			s = s >> 1;
		}
	}
		break;
	case SUBTRIE:
	case SUBTRIE_REQUEST: {
		uint64_t s = size_ - 1;
		for (int i = 7; i >= 2; i--) {
			if (s % 2 == 1) {
				BITSET(buffer, i);
			}
			s = s >> 1;
		}
	}
		break;
	default:
		throw "Unsupported";
	}
}

bool PacketHeader::isInputHeaderComplete() const {
	if (t_ == DATA || t_ == FILTER || t_ == COMPRESSED_FILTER) {
		if (this->inHeaderPos_ >= 8) {
			return true;
		} else {
			return false;
		}
	} else if (t_ == UNDEFINED) {
		throw "Unsupported";
	} else {
		return true;
	}
}

std::size_t PacketHeader::getPacketSize() const {
	return this->size_;
}

PacketHeader::Type PacketHeader::getType() const {
	return this->t_;
}

bool InPacket::awaitingInput() const {
	if (!isInputHeaderComplete()) {
		return true;
	}
	//TODO
	throw "";
}
}
}
