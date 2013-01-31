/*
 *
 * @author	Till Lorentzen
 */

#ifndef CRYPTOHASH_H_
#define CRYPTOHASH_H_

#define DEFAULT_DIGIT_ALGORITHM "sha1"

#include <string>
#include <istream>
namespace setsync {
/**
 * All cryptographic hash functions are at this namespace
 */
namespace crypto {
/**
 * Wrapper class to provide a generic interface for polarssl and openssl
 */
class CryptoHash {
private:
	std::string name_;
	std::size_t size_;
	const void * digit;
	/**
	 * Hashes the given input string into the given target_md
	 * buffer
	 *
	 * \param target_md pointer to an array to save the finished hash into
	 * \param str to be hashed into the target_md
	 * \return true on success and false on fail
	 */
	int hash(unsigned char * target_md, const char * str) const;
	/**
	 * Hashes the given data with the given length into the given target_md
	 * buffer
	 *
	 * \param target_md pointer to an array to save the finished hash into
	 * \param data to be hashed into the target_md
	 * \param length of the data array
	 * \return true on success and false on fail
	 */
	int hash(unsigned char * target_md, const unsigned char * data,
			const std::size_t length) const;
	/**
	 * Hashes the given input string into the given target_md
	 * buffer
	 *
	 * \param target_md pointer to an array to save the finished hash into
	 * \param str to be hashed into the target_md
	 * \return true on success and false on fail
	 */
	int hash(unsigned char * target_md, const std::string& str) const;
	/**
	 * Hashes the given input stream into the given target_md
	 * buffer
	 *
	 * \param target_md pointer to an array to save the finished hash into
	 * \param in input stream to be hashed into the target_md
	 * \return true on success and false on fail
	 */
	int hash(unsigned char * target_md, std::istream& in) const;
	/**
	 * Initializes the needed data structures for later usage
	 *
	 * It will only be called by the constructor
	 *
	 */
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
	/**
	 * copy constructor
	 *
	 * \param copy the instance which should be copied
	 */
	CryptoHash(const CryptoHash& copy);
	/**
	 * default destructor
	 */
	virtual ~CryptoHash();
	/**
	 * \return memory size of the hash digit in bytes
	 */
	std::size_t getHashSize() const;
	/**
	 * Hashes the given input string into the given target_md
	 * buffer
	 *
	 * \param target_md pointer to an array to save the finished hash into
	 * \param str to be hashed into the target_md
	 * \return true on success and false on fail
	 */
	int operator()(unsigned char * target_md, const char * str) const;
	/**
	 * Hashes the given data with the given length into the given target_md
	 * buffer
	 *
	 * \param target_md pointer to an array to save the finished hash into
	 * \param data to be hashed into the target_md
	 * \param length of the data array
	 * \return true on success and false on fail
	 */
	int operator()(unsigned char * target_md, const unsigned char * data,
			const std::size_t length) const;
	/**
	 * Hashes the given input stream into the given target_md
	 * buffer
	 *
	 * \param target_md pointer to an array to save the finished hash into
	 * \param in input stream to be hashed into the target_md
	 * \return true on success and false on fail
	 */
	int operator()(unsigned char * target_md, const std::string& str) const;
	/**
	 * Hashes the given input stream into the given target_md
	 * buffer
	 *
	 * \param target_md pointer to an array to save the finished hash into
	 * \param in input stream to be hashed into the target_md
	 * \return true on success and false on fail
	 */
	int operator()(unsigned char * target_md, std::istream& in) const;
	/**
	 * \return the name of the chosen crypto hash algorithm
	 */
	std::string getName(void) const;
};
/**
 *
 */
class CryptoHashContainer {
private:
	/// pointer to the allocated array where the Message Digit is stored
	unsigned char * md_;
	/// crypto hash function which has been used to create the Message Digit
	const CryptoHash& hashFunction_;
public:
	/**
	 * Creates
	 *
	 * \param hash function used for the data of the container
	 * \param md to be saved into the container
	 */
			CryptoHashContainer(const CryptoHash& hash,
					const unsigned char * md = NULL);
	/**
	 * Copies the given container values into a new instance
	 *
	 * \param other Container, which should be copied
	 */
	CryptoHashContainer(const CryptoHashContainer& other);
	/**
	 * destructor, which cleans the allocated space at md_
	 */
	virtual ~CryptoHashContainer();
	/**
	 * \param md to be saved into the Container
	 */
	void set(const unsigned char * md);
	/**
	 * Returns a const pointer to the saved Message Digit of this
	 * Container
	 *
	 * \return a pointer to the saved message digit
	 */
	const unsigned char * get(void) const;
	/**
	 * Compares the given Container hash functions and its message digits
	 *
	 * \param other container, which should be compared
	 * \returns true if both are equal, otherwise false
	 */
	bool operator ==(const CryptoHashContainer& other) const;
	/**
	 * Compares the given Container hash functions and its message digits
	 *
	 * \param other container, which should be compared
	 * \returns true if both are different, otherwise false
	 */
	bool operator !=(const CryptoHashContainer& other) const;
	/**
	 * Compares the given message digits with the internal message
	 *
	 * \param other message digit, which should be compared
	 * \returns true if both are equal, otherwise false
	 */
	bool operator ==(const unsigned char * other) const;
	/**
	 * Compares the given message digits with the internal message
	 *
	 * \param other message digit, which should be compared
	 * \returns true if both are different, otherwise false
	 */
	bool operator !=(const unsigned char * other) const;
};

}
}
#endif /* CRYPTOHASH_H_ */
