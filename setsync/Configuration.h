/*
 * Config.h
 *
 *      Author: Till Lorentzen
 */

#ifndef CONFIG_H_
#define CONFIG_H_
#include <cstddef>
#include <stdint.h>
#include <string>

namespace setsync {

class Configuration {
public:
	Configuration(const uint64_t maxNumberOfElements = 10000,
			const bool hardMaximum = false,
			const float falsePositiveRate = 0.001,
			const char * hashname = "sha1") :
		bloomFilterHardMaximum(hardMaximum), hashname(hashname) {
	}
	uint64_t bloomFilterMaxElements;
	bool bloomFilterHardMaximum;
	float bloomFilterFalsePositiveRate;
	const std::string hashname;
};

}

#endif /* CONFIG_H_ */
