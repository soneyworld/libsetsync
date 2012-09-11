/*
 * Main.cpp
 *
 *      Author: Till Lorentzen
 */

#include "SHA1Generator.h"

int main(int ac, char **av) {
	uint64_t stepsize = 100000000;
	for (int i = 0; i < 2; i++) {
		SHA1Generator generator(i * stepsize, i * stepsize + stepsize);
		generator.run();
	}
}
