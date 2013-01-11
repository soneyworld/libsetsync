/*
 *      Author: Till Lorentzen
 */
#include "CryptoHash.h"
#include <string.h>
#ifdef HAVE_OPENSSL
#include <openssl/sha.h>
#endif
namespace setsync {
namespace crypto {

void CryptoHash::init() {
#ifdef HAVE_OPENSSL
	OpenSSL_add_all_digests();
	digit = EVP_get_digestbyname(name_.c_str());
	if (digit == NULL) {
		throw "Crypto hash algorithm not found";
	}
#else
	this->digit = md_info_from_string(name_.c_str());
	if (this->digit == NULL) {
		throw "Crypto hash algorithm not found";
	}
#endif

#ifdef HAVE_OPENSSL
	size_ = this->digit->md_size;
#else
	size_ = this->digit->size;
#endif

}

CryptoHash::CryptoHash(const char * name) :
	name_(name) {
	init();
}

CryptoHash::CryptoHash(const std::string& name) :
	name_(name) {
	init();
}

CryptoHash::CryptoHash(const CryptoHash& copy) :
	name_(copy.getName()) {
	init();
}

std::size_t CryptoHash::getHashSize() const {
	return this->size_;
}

int CryptoHash::hash(unsigned char * target_md, const char *str) const {
#ifdef HAVE_OPENSSL
	int result = 0;
	unsigned int md_len;
	EVP_MD_CTX mdctx;
	EVP_MD_CTX_init(&mdctx);
	result = EVP_DigestInit_ex(&mdctx, digit, NULL);
	result = EVP_DigestUpdate(&mdctx, str, strlen(str));
	result = EVP_DigestFinal_ex(&mdctx, target_md, &md_len);
	result = EVP_MD_CTX_cleanup(&mdctx);
	return result == 1;
#else
	return md(this->digit, (const unsigned char*) str, strlen(str), target_md);
#endif
}

int CryptoHash::hash(unsigned char * target_md, const unsigned char *data,
		const size_t length) const {
#ifdef HAVE_OPENSSL
	int result = 0;
	unsigned int md_len;
	EVP_MD_CTX mdctx;
	EVP_MD_CTX_init(&mdctx);
	result = EVP_DigestInit_ex(&mdctx, digit, NULL);
	result = EVP_DigestUpdate(&mdctx, data, length);
	result = EVP_DigestFinal_ex(&mdctx, target_md, &md_len);
	result = EVP_MD_CTX_cleanup(&mdctx);
	return result == 1;
#else
	return md(this->digit, data, length, target_md);
#endif
}

int CryptoHash::hash(unsigned char * target_md, const std::string& str) const {
#ifdef HAVE_OPENSSL
	int result = 0;
	unsigned int md_len;
	EVP_MD_CTX mdctx;
	EVP_MD_CTX_init(&mdctx);
	result = EVP_DigestInit_ex(&mdctx, digit, NULL);
	result = EVP_DigestUpdate(&mdctx, str.c_str(), str.size());
	result = EVP_DigestFinal_ex(&mdctx, target_md, &md_len);
	result = EVP_MD_CTX_cleanup(&mdctx);
	return result == 1;
#else
	return md(this->digit, (const unsigned char*) str.c_str(), str.size(),
			target_md);
#endif
}

int CryptoHash::hash(unsigned char * target_md, std::istream& in) const {
	int result = 0;
#ifdef HAVE_OPENSSL
	unsigned int md_len;
	EVP_MD_CTX mdctx;
	EVP_MD_CTX_init(&mdctx);
	result = EVP_DigestInit_ex(&mdctx, digit, NULL);
	char ch;
	while (in.get(ch) && result == 1) {
		result = EVP_DigestUpdate(&mdctx, &ch, 1);
	}
	result = EVP_DigestFinal_ex(&mdctx, target_md, &md_len);
	result = EVP_MD_CTX_cleanup(&mdctx);
	return result == 1;
#else
	md_context_t mdctx;
	result = md_init_ctx(&mdctx, digit);
	result = md_starts(&mdctx);
	char ch;
	while (in.get(ch) && result == 1) {
		result = md_update(&mdctx, (unsigned char *) &ch, 1);
	}
	result = md_finish(&mdctx, target_md);
	result = md_free_ctx(&mdctx);
	return result;
#endif
}

CryptoHash::~CryptoHash() {
}

std::string CryptoHash::getDefaultName() {
	return std::string(DEFAULT_DIGIT_ALGORITHM);
}

std::size_t CryptoHash::getDefaultDigitLength() {
	const CryptoHash hash;
	return hash.getHashSize();
}

int CryptoHash::operator()(unsigned char * target_md, const char * str) const {
	return hash(target_md, str);
}
int CryptoHash::operator()(unsigned char * target_md,
		const unsigned char * data, const std::size_t length) const {
	return hash(target_md, data, length);
}
int CryptoHash::operator()(unsigned char * target_md, const std::string& str) const {
	return hash(target_md, str);
}
int CryptoHash::operator()(unsigned char * target_md, std::istream& in) const {
	return hash(target_md, in);
}

std::string CryptoHash::getName() const {
	return std::string(name_);
}

CryptoHashContainer::CryptoHashContainer(const CryptoHash& hash,
		const unsigned char * md) :
	hashFunction_(hash) {
	this->md_ = new unsigned char[hashFunction_.getHashSize()];
	if (md != NULL) {
		memcpy(md_, md, hashFunction_.getHashSize());
	} else {
		memset(md_, 0, hashFunction_.getHashSize());
	}
}
CryptoHashContainer::~CryptoHashContainer() {
	delete[] this->md_;
}

void CryptoHashContainer::set(const unsigned char * md) {
	memcpy(md_, md, hashFunction_.getHashSize());
}
const unsigned char * CryptoHashContainer::get(void) const {
	return this->md_;
}

CryptoHashContainer::CryptoHashContainer(const CryptoHashContainer& other) :
	hashFunction_(other.hashFunction_) {
	this->md_ = new unsigned char[hashFunction_.getHashSize()];
	memcpy(md_, other.md_, hashFunction_.getHashSize());
}

bool CryptoHashContainer::operator ==(const CryptoHashContainer& other) const {
	if (other.hashFunction_.getHashSize() != hashFunction_.getHashSize())
		return false;
	if (other.hashFunction_.getName() != hashFunction_.getName()) {
		return false;
	}
	return memcmp(md_, other.md_, hashFunction_.getHashSize()) == 0;
}
bool CryptoHashContainer::operator !=(const CryptoHashContainer& other) const {
	return !(*this == other);
}

bool CryptoHashContainer::operator ==(const unsigned char * other) const {
	return memcmp(md_, other, hashFunction_.getHashSize()) == 0;
}
bool CryptoHashContainer::operator !=(const unsigned char * other) const {
	return memcmp(md_, other, hashFunction_.getHashSize()) != 0;
}
}
}
