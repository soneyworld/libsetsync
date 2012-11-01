/*
 *      Author: Till Lorentzen
 */
#include "CryptoHash.h"
#include <string.h>
#ifdef HAVE_OPENSSL
#include <openssl/sha.h>
#endif
namespace utils {

CryptoHash::CryptoHash(const char * name) {
#ifdef HAVE_OPENSSL
	OpenSSL_add_all_digests();
	digit = EVP_get_digestbyname(name);
	if (!digit) {
		throw "Crypto hash algorithm not found";
	}
#else

#endif
}

const std::size_t CryptoHash::getHashSize() const {
#ifdef HAVE_OPENSSL
	return this->digit->md_size;
#else
	return this->hashSize;
#endif
}

int CryptoHash::hash(unsigned char * target_md, const char *str) const {
	int result = 0;
#ifdef HAVE_OPENSSL
	unsigned int md_len;
	EVP_MD_CTX mdctx;
	EVP_MD_CTX_init(&mdctx);
	result = EVP_DigestInit_ex(&mdctx, digit, NULL);
	result = EVP_DigestUpdate(&mdctx, str, strlen(str));
	result = EVP_DigestFinal_ex(&mdctx, target_md, &md_len);
	result = EVP_MD_CTX_cleanup(&mdctx);
	return result == 1;
#else

#endif
}

int CryptoHash::hash(unsigned char * target_md, const unsigned char *data,
		const size_t length) const {
	int result = 0;
#ifdef HAVE_OPENSSL
	unsigned int md_len;
	EVP_MD_CTX mdctx;
	EVP_MD_CTX_init(&mdctx);
	result = EVP_DigestInit_ex(&mdctx, digit, NULL);
	result = EVP_DigestUpdate(&mdctx, data, length);
	result = EVP_DigestFinal_ex(&mdctx, target_md, &md_len);
	result = EVP_MD_CTX_cleanup(&mdctx);
	return result == 1;
#else

#endif
}

int CryptoHash::hash(unsigned char * target_md, const std::string& str) const {
	int result = 0;
#ifdef HAVE_OPENSSL
	unsigned int md_len;
	EVP_MD_CTX mdctx;
	EVP_MD_CTX_init(&mdctx);
	result = EVP_DigestInit_ex(&mdctx, digit, NULL);
	result = EVP_DigestUpdate(&mdctx, str.c_str(), str.size());
	result = EVP_DigestFinal_ex(&mdctx, target_md, &md_len);
	result = EVP_MD_CTX_cleanup(&mdctx);
	return result == 1;
#else

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

#endif
}

CryptoHash::~CryptoHash() {

}

std::size_t CryptoHash::getDefaultDigitLength() {
#ifdef HAVE_OPENSSL
	return SHA_DIGEST_LENGTH;
#else
	return 20;
#endif
}

int CryptoHash::operator()(unsigned char * target_md, const char * str) const {
	return hash(target_md, str);
}
int CryptoHash::operator()(unsigned char * target_md, const unsigned char * data,
		const std::size_t length) const {
	return hash(target_md, data, length);
}
int CryptoHash::operator()(unsigned char * target_md, const std::string& str) const {
	return hash(target_md, str);
}
int CryptoHash::operator()(unsigned char * target_md, std::istream& in) const {
	return hash(target_md, in);
}
}
