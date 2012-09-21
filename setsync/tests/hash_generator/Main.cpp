/*
 * Main.cpp
 *
 *      Author: Till Lorentzen
 */
#include <omp.h>
#include <iostream>
#include "SHA1Generator.h"
#include <setsync/bloom/BloomFilter.h>

int main(int ac, char **av) {
	uint64_t stepsize = 100000;
	bloom::BloomFilter all(100000000);
	//	omp_set_num_threads(1);
	omp_lock_t lock;
	omp_init_lock(&lock);
#pragma omp parallel for schedule(static)
	for (int i = 0; i < 1000; i++) {
		SHA1Generator generator(i * stepsize, i * stepsize + stepsize);
		generator.run();
		bloom::BloomFilter bf(100000000);
		for (int j = 0; j < stepsize; j++) {
			bf.add(generator.array + (j * 20));
		}
		omp_set_lock(&lock);
		all|=bf;
		omp_unset_lock(&lock);
		//		std::cout << "Thread number: " << omp_get_thread_num() << std::endl;
	}
}
