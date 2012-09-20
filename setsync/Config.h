/*
 * Config.h
 *
 *      Author: Till Lorentzen
 */

#ifndef CONFIG_H_
#define CONFIG_H_
#include <setsync/sha1.h>
#include <cstddef>
#include <stdint.h>

namespace setsync {

class Config {
public:
	Config(const uint64_t maxNumberOfElements = 10000,
			const bool hardMaximum = false,
			const float falsePositiveRate = 0.001,
			const std::size_t hashsize = SHA_DIGEST_LENGTH):bloomFilterHardMaximum(hardMaximum), hashSize(hashsize){}
	uint64_t bloomFilterMaxElements;
	bool bloomFilterHardMaximum;
	float bloomFilterFalsePositiveRate;
	const size_t hashSize;
};

}

#endif /* CONFIG_H_ */
