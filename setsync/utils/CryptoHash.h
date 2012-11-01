/*
 *
 *      Author: Till Lorentzen
 */

#ifndef CRYPTOHASH_H_
#define CRYPTOHASH_H_
#include "config.h"
#include <string>
#include <istream>
#ifdef HAVE_OPENSSL
#include <openssl/evp.h>
#else
#include "md.h"
#endif
namespace utils{

class CryptoHash{
private:
#ifdef HAVE_OPENSSL
	const EVP_MD *digit;
#else
	const md_info_t *digit;
#endif
	int hash(unsigned char * target_md, const char * str) const;
	int hash(unsigned char * target_md, const unsigned char * data, const std::size_t length) const;
	int hash(unsigned char * target_md, const std::string& str) const;
	int hash(unsigned char * target_md, std::istream& in) const;
public:
	CryptoHash(const char * name = "md5");
	virtual ~CryptoHash();
	const std::size_t getHashSize() const;
	static std::size_t getDefaultDigitLength();
	int operator()(unsigned char * target_md, const char * str) const;
	int operator()(unsigned char * target_md, const unsigned char * data, const std::size_t length) const;
	int operator()(unsigned char * target_md, const std::string& str) const;
	int operator()(unsigned char * target_md, std::istream& in) const;
};

}

#endif /* CRYPTOHASH_H_ */
