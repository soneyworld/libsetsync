/*
 * HashFunction.cpp
 *
 *      Author: Till Lorentzen
 */

#include "HashFunction.h"

namespace bloom {

HashFunctionFactory& HashFunctionFactory::getInstance() {
	static HashFunctionFactory instance;
	return instance;
}

HashFunction * HashFunctionFactory::createHashFunction(std::string function) {
	return new SaltedHashFunction(64);
}

SplittingMDHashFunction::SplittingMDHashFunction(
		const std::size_t outputLength, const std::size_t hashSize) :
	hashSize_(hashSize), outputLength_(outputLength) {

}

SplittingMDHashFunction::~SplittingMDHashFunction() {
}

size_t SplittingMDHashFunction::count() const {
	//TODO
	throw "MUST BE IMPLEMENTED";
}

uint64_t SplittingMDHashFunction::hash(const unsigned char * input,
		const std::size_t length) const {
	return hash(input,length,0);
}

uint64_t SplittingMDHashFunction::hash(const unsigned char * input,
		const std::size_t length, const std::size_t function) const {
	//TODO
	throw "MUST BE IMPLEMENTED";
}

SaltedHashFunction::SaltedHashFunction(size_t salt_count) :
	salt_count_(salt_count) {
	generate_salt();
}

SaltedHashFunction::~SaltedHashFunction() {

}

size_t SaltedHashFunction::count() const {
	return _salt.size();
}

uint64_t SaltedHashFunction::hash(const unsigned char * input,
		const std::size_t length) const {
	return hash(input, length, 0);
}

uint64_t SaltedHashFunction::hash(const unsigned char * input,
		const std::size_t length, const std::size_t function) const {
	const unsigned char* itr = input;
	std::size_t remaining_length = length;
	uint64_t hash = this->_salt[function];
	uint64_t loop = 0;
	while (remaining_length >= 8) {
		const uint32_t& i1 = *(reinterpret_cast<const uint32_t*> (itr));
		itr += sizeof(uint32_t);
		const uint32_t& i2 = *(reinterpret_cast<const uint32_t*> (itr));
		itr += sizeof(uint32_t);
		hash ^= (hash << 7) ^ i1 * (hash >> 3) ^ (~((hash << 11) + (i2 ^ (hash
				>> 5))));
		remaining_length -= 8;
	}
	while (remaining_length >= 4) {
		const uint32_t& i = *(reinterpret_cast<const uint32_t*> (itr));
		if (loop & 0x01)
			hash ^= (hash << 7) ^ i * (hash >> 3);
		else
			hash ^= (~((hash << 11) + (i ^ (hash >> 5))));
		++loop;
		remaining_length -= 4;
		itr += sizeof(uint32_t);
	}
	while (remaining_length >= 2) {
		const uint16_t& i = *(reinterpret_cast<const uint16_t*> (itr));
		if (loop & 0x01)
			hash ^= (hash << 7) ^ i * (hash >> 3);
		else
			hash ^= (~((hash << 11) + (i ^ (hash >> 5))));
		++loop;
		remaining_length -= 2;
		itr += sizeof(uint16_t);
	}
	if (remaining_length) {
		hash += ((*itr) ^ (hash * 0xA5A5A5A5)) + loop;
	}
	return hash;
}

void SaltedHashFunction::generate_salt() {
	const uint32_t predef_salt_count = 64;
	static const int predef_salt[predef_salt_count] = { 0xAAAAAAAA, 0x55555555,
			0x33333333, 0xCCCCCCCC, 0x66666666, 0x99999999, 0xB5B5B5B5,
			0x4B4B4B4B, 0xAA55AA55, 0x55335533, 0x33CC33CC, 0xCC66CC66,
			0x66996699, 0x99B599B5, 0xB54BB54B, 0x4BAA4BAA, 0xAA33AA33,
			0x55CC55CC, 0x33663366, 0xCC99CC99, 0x66B566B5, 0x994B994B,
			0xB5AAB5AA, 0xAAAAAA33, 0x555555CC, 0x33333366, 0xCCCCCC99,
			0x666666B5, 0x9999994B, 0xB5B5B5AA, 0xFFFFFFFF, 0xFFFF0000,
			0xB823D5EB, 0xC1191CDF, 0xF623AEB3, 0xDB58499F, 0xC8D42E70,
			0xB173F616, 0xA91A5967, 0xDA427D63, 0xB1E8A2EA, 0xF6C0D155,
			0x4909FEA3, 0xA68CC6A7, 0xC395E782, 0xA26057EB, 0x0CD5DA28,
			0x467C5492, 0xF15E6982, 0x61C6FAD3, 0x9615E352, 0x6E9E355A,
			0x689B563E, 0x0C9831A8, 0x6753C18B, 0xA622689B, 0x8CA63C47,
			0x42CC2884, 0x8E89919B, 0x6EDBD7D3, 0x15B6796C, 0x1D6FDFE4,
			0x63FF9092, 0xE7401432 };

	if (salt_count_ > predef_salt_count) {
		throw "Max. 64 hash salts supported!";
	}

	for (unsigned int i = 0; i < salt_count_; ++i) {
		_salt.push_back(predef_salt[i]);
	}
}

}
