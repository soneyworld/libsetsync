/*
 *
 * @author	Till Lorentzen
 */

#ifndef CRYPTOHASH_H_
#define CRYPTOHASH_H_

#define DEFAULT_DIGIT_ALGORITHM "sha1"
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
	std::string name_;
	std::size_t size_;
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
	void init(void);
public:
	/**
	 * \return the length of the default digit algorithm
	 */
	static std::size_t getDefaultDigitLength();
	/**
	 * \return the name of the default digit algorithm
	 */
	static std::string getDefaultName();
	/**
	 * Possible hash algorithms are: md2, md4, md5, sha1, sha224, sha256, sha384, sha512
	 * The default algorithm is sha1
	 *
	 * \param name of the hash algorithm
	 */
	CryptoHash(const char * name = DEFAULT_DIGIT_ALGORITHM);
	/**
	 * Possible hash algorithms are: md2, md4, md5, sha1, sha224, sha256, sha384, sha512
	 * The default algorithm is sha1
	 *
	 * \param name of the hash algorithm
	 */
	CryptoHash(const std::string& name);

	CryptoHash(const CryptoHash& copy);
	/**
	 * default destructor
	 */
	virtual ~CryptoHash();
	/**
	 * \return memory size of the hash digit in bytes
	 */
	std::size_t getHashSize() const;

	int operator()(unsigned char * target_md, const char * str) const;
	int operator()(unsigned char * target_md, const unsigned char * data,
			const std::size_t length) const;
	int operator()(unsigned char * target_md, const std::string& str) const;
	int operator()(unsigned char * target_md, std::istream& in) const;
	/**
	 * \return the name of the chosen crypto hash algorithm
	 */
	std::string getName(void) const;
};

class CryptoHashContainer {
private:
	unsigned char * md_;
	const CryptoHash& hashFunction_;
public:
			CryptoHashContainer(const CryptoHash& hash,
					const unsigned char * md = NULL);
	virtual ~CryptoHashContainer();
	void set(const unsigned char * md);
	const unsigned char * get(void) const;
	CryptoHashContainer(const CryptoHashContainer& other);
	bool operator ==(const CryptoHashContainer& other) const;
	bool operator !=(const CryptoHashContainer& other) const;
	bool operator ==(const unsigned char * other) const;
	bool operator !=(const unsigned char * other) const;
};

}

#endif /* CRYPTOHASH_H_ */
