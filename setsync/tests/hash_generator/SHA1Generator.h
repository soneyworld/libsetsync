/*
 * SHA1Generator.h
 *
 *      Author: Till Lorentzen
 */

#ifndef SHA1GENERATOR_H_
#define SHA1GENERATOR_H_
#include <stdint.h>

class SHA1Generator {
private:
	uint64_t start;
	uint64_t end;
	unsigned char * array;
public:
	SHA1Generator(int start, int end);
	void run();
	virtual ~SHA1Generator();
};

#endif /* SHA1GENERATOR_H_ */
