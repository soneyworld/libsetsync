/**
 * DoubleHashingScheme.h
 *
 *      Author: Till Lorentzen
 *
 *      The DoubleHashing class is an adaptation of a hashing mechanism
 *      explained at the paper: "Less Hashing, Same Performance: Building a
 *      Better Bloom Filter" written by Adam Kirsch and Michael Mitzenmacher.
 *
 *      It takes the given cryptographically hash key and splits it up to two
 *      values, which are taken as f_1 and f_2. The following formula produces
 *      a parameterized hash function g_i:
 *
 *      g_i(HASH) = f_1 + i * f_2
 *
 *      The extended Version takes another function f_3 and takes the parameter i
 *      as input:
 *
 *      g_i(HASH) = f_1 + i * f_2 + f_3(i)
 *
 */

#ifndef DOUBLEHASHINGSCHEME_H_
#define DOUBLEHASHINGSCHEME_H_

#include "HashFunction.h"
#include "setsync/sha1.h"

namespace bloom {
/**
 * The DoubleHashingScheme takes a cryptographically hash key as input and produces an
 * unlimited set of hash functions.
 */
class DoubleHashingScheme: public HashFunction {
public:
	/**
	 *  Default constructor takes the length of the given cryptographically input key
	 */
	DoubleHashingScheme(const std::size_t hashsize = SHA_DIGEST_LENGTH);
	/// Default destructor
	virtual ~DoubleHashingScheme();
	/**
	 * \param input pointer to the input hash key
	 * \param length length of the input hash key
	 * \throws an exception, if the length!=hashsize
	 * \return a 64 bit hash as integer
	 */
	virtual uint64_t
	hash(const unsigned char * input, const std::size_t length) const;
	/**
	 * \param input pointer to the input hash key
	 * \param length length of the input hash key
	 * \param function the hash function to be choosen
	 * \throws an exception, if the length!=hashsize
	 * \return a 64 bit hash as integer
	 */
	virtual uint64_t hash(const unsigned char * input,
			const std::size_t length, const std::size_t function) const;
	/**
	 * Because this hash function is not limited in functions, the max of std::size_t
	 * will be returned
	 * \return max value of std::size_t
	 */
	virtual std::size_t count() const;
protected:
	/// Size of the given hash key in bytes
	const std::size_t hashsize_;

};

/**
 * ExtendedDoubleHashingScheme is a little modification of the standard
 * DoubleHashingScheme. It adds a little third function to the hash function.
 */
class ExtendedDoubleHashingScheme: public DoubleHashingScheme {
public:
	/**
	 *  Default constructor takes the length of the given cryptographically input key
	 */
	ExtendedDoubleHashingScheme(const std::size_t hashsize = SHA_DIGEST_LENGTH);
	/// Default destructor
	virtual ~ExtendedDoubleHashingScheme();
	virtual uint64_t hash(const unsigned char * input,
			const std::size_t length, const std::size_t function) const;
private:
	/**
	 * \param input pointer to the position in the input key, which hasn't been used
	 * \param remaining number of unused bytes in the input key
	 * \param function the number of the actual used function
	 * \return a uint64 value generated on the input and function counter
	 */
	uint64_t extendedHash(const unsigned char * input,
			const std::size_t remaining, const std::size_t function) const;
};
}

#endif /* DOUBLEHASHINGSCHEME_H_ */
