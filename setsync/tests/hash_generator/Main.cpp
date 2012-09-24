/*
 * Main.cpp
 *
 *      Author: Till Lorentzen
 */
#include <omp.h>
#include <iostream>
#include "SHA1Generator.h"
#include <time.h>
#include <stdio.h>
//#include <setsync/bloom/BloomFilter.h>
#include <setsync/index/SQLiteDatabase.h>
#include <setsync/index/SQLiteIndex.h>
#include <setsync/bloom/HashFunction.h>

int main(int ac, char **av) {
	uint64_t stepsize = 100000;
	//	bloom::BloomFilter all(100000000);
	std::string s = "dfg";
	std::string dbname = "generated.db";
//	sqlite::SQLiteDatabase * db = new sqlite::SQLiteDatabase(dbname);
	sqlite::SQLiteDatabase * db = new sqlite::SQLiteDatabase();
	sqlite::SQLiteIndex index(db, s);
	//	omp_set_num_threads(1);
	omp_lock_t lock;
	omp_init_lock(&lock);
	unsigned int counter = 0;
	unsigned int icounter = 0;
	int thread_id;
	time_t seconds;
	seconds = time(NULL);
	time_t duration = 0;
	time_t iduration = 0;
#pragma omp parallel for schedule(static) private(thread_id)
	for (int i = 0; i < 1000; i++) {
		thread_id = omp_get_thread_num();
		bloom::SaltedHashFunction function(10);
		SHA1Generator generator(i * stepsize, i * stepsize + stepsize);
		generator.run();

		//bloom::BloomFilter bf(100000000);
		//for (int j = 0; j < stepsize; j++) {
		//	bf.add(generator.array + (j * 20));
		//}

		//all|=bf;
		for (uint64_t k = 0; k < stepsize; k++) {
			for (unsigned int j = 0; j < function.count(); j++) {
				uint64_t hash =
						function.hash(generator.array + (k * 20), 20, j);
				omp_set_lock(&lock);
				index.insert(hash, generator.array + (k * 20), 20);
				counter++;
				icounter++;
				omp_unset_lock(&lock);
			}
			if (thread_id == 0) {
				if (seconds + 10 < time(NULL)) {
					omp_set_lock(&lock);
					index.commit();
					omp_unset_lock(&lock);
					duration += time(NULL) - seconds;
					iduration = time(NULL) - seconds;
					printf("%d inserts after %ld sec (%ld per sec) (%ld per sec in last interval)\n", counter, duration, counter/duration, icounter/iduration);
					icounter = 0;
					seconds = time(NULL);
				}
			}
		}

		//		std::cout << "Thread number: " << omp_get_thread_num() << std::endl;

	}
	index.commit();
	printf("%d inserts after %ld sec (%ld per sec)\n", counter, duration, counter/duration);
}
