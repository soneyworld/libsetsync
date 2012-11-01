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
	this->digit = md_info_from_string(name);
	if (this->digit == NULL) {
		throw "Crypto hash algorithm not found";
	}
#endif
}

const std::size_t CryptoHash::getHashSize() const {
#ifdef HAVE_OPENSSL
	return this->digit->md_size;
#else
	return this->digit->size;
#endif
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
		result = md_update(&mdctx,(unsigned char *) &ch, 1);
	}
	result = md_finish(&mdctx, target_md);
	result = md_free_ctx(&mdctx);
	return result;
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
}
