/*
 * ComparableBloomFilter.h
 *
 *      Author: Till Lorentzen
 */

#ifndef COMPARABLEBLOOMFILTER_H_
#define COMPARABLEBLOOMFILTER_H_
#include <setsync/DiffHandler.h>

namespace bloom {
/**
 * All bloom filter, which implement this, must have the ability
 * to return the hashes, which has been lead to a "1" at a specific
 * position of the bloom filter. Because if a remote bloom filter
 * has got a "0" at a specific position. The hashes, which has been
 * lead to the local "1" must be returned.
 */
class ComparableBloomFilterInterface {
public:
	/**
	 * This method provides a comparison between the given external bloom
	 * filter segment and the corresponding local bloom filter segment. The
	 * given length of the segment has to be the length in bytes of the
	 * given external bloom filter segment. The given offset shows at which
	 * position in the internal bloom filter should be started. The offset
	 * is also given in bytes. If a local bit in the segment is "1" and in
	 * the external segment, this position is "0", then call the handler
	 * with the hash, lead to this local "1".
	 *
	 * \param externalBF segment of an external bloom filter
	 * \param length of the given segment in bytes
	 * \param offset from the start of the external bloom filter in bytes
	 * \param handler to be called on a local hash, which lacks remotely
	 */
	virtual void diff(const unsigned char * externalBF,
			const std::size_t length, const std::size_t offset,
			setsync::AbstractDiffHandler& handler) const = 0;
	/**
	 * This method will write the requested chunk of the bloom filter
	 * to the given buffer, it start at the given offset and writes a
	 * maximum length of the given length into the buffer. The method
	 * must return the size of the written data. It the returned size is
	 * less than the given size, the end if the bloom filter data is reached
	 * this can be happen, if the bloom filter size has been reached,
	 * or, if the bloomfilter is compressed, it can happen before the
	 * size of the bloom filter is reached.
	 */
	virtual std::size_t getChunk(unsigned char * buffer,
			const std::size_t maxlength, std::size_t offset) = 0;
};

}

#endif /* COMPARABLEBLOOMFILTER_H_ */
