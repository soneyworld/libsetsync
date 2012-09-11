/*
 * Main.cpp
 *
 *      Author: Till Lorentzen
 */
#include <omp.h>
#include <iostream>
#include "SHA1Generator.h"


int main(int ac, char **av) {
	uint64_t stepsize = 1000000;
#pragma omp parallel for
	for (int i = 0; i < 1000; i++) {
		SHA1Generator generator(i * stepsize, i * stepsize + stepsize);
		generator.run();
//		std::cout << "Thread number: " << omp_get_thread_num() << std::endl;
	}
}
