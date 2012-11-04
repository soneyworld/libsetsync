/*
 *
 * @author	Till Lorentzen
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
namespace utils {
/**
 * Wrapper class to provide a generic interface for polarssl and openssl
 */
class CryptoHash {
private:
#ifdef HAVE_OPENSSL
	/// Hash algorithm properties of OPENSSL
	const EVP_MD *digit;
#else
	/// Hash algorithm properties of polarssl
	const md_info_t *digit;
#endif
	int hash(unsigned char * target_md, const char * str) const;
	int hash(unsigned char * target_md, const unsigned char * data,
			const std::size_t length) const;
	int hash(unsigned char * target_md, const std::string& str) const;
	int hash(unsigned char * target_md, std::istream& in) const;
public:
	/**
	 * Possible hash algorithms are: md2, md4, md5, sha1, sha224, sha256, sha384, sha512
	 *
	 * \param name of the hash algorithm
	 */
	CryptoHash(const char * name = "md5");
	/**
	 * default destructor
	 */
	virtual ~CryptoHash();
	/**
	 * \return memory size of the hash digit in bytes
	 */
	const std::size_t getHashSize() const;
	/**
	 * \return 20
	 */
	static std::size_t getDefaultDigitLength();
	int operator()(unsigned char * target_md, const char * str) const;
	int operator()(unsigned char * target_md, const unsigned char * data,
			const std::size_t length) const;
	int operator()(unsigned char * target_md, const std::string& str) const;
	int operator()(unsigned char * target_md, std::istream& in) const;
};

}

#endif /* CRYPTOHASH_H_ */
