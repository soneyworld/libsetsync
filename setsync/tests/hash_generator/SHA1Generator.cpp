/*
 * SHA1Generator.cpp
 *
 *      Author: Till Lorentzen
 */

#include <openssl/sha.h>

#include "SHA1Generator.h"
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string.h>

SHA1Generator::SHA1Generator(int start, int end) {
	this->start = start;
	this->end = end;
	this->array = (unsigned char*) malloc(20 * (end - start));
	if (this->array == NULL) {
		this->end = this->start;
	} else {
		memset(this->array, 0, 20 * (end - start));
	}
}

SHA1Generator::~SHA1Generator() {
	/*uint64_t pos;
	 for (pos = 0; pos < end - start; pos++) {
	 std::stringstream s;
	 s.fill('0');
	 for (size_t i = 0; i < 20; ++i) {
	 s << std::setw(2) << std::hex << (unsigned short) this->array[i
	 + 20 * pos];
	 }
	 std::cout << s.str() << std::endl;
	 }*/
	if (this->array != NULL)
		free(this->array);
}

void SHA1Generator::run() {
	uint64_t i;
	SHA_CTX ctx;
	for (i = this->start; i < this->end; i++) {
		SHA1_Init(&ctx);
		SHA1_Update(&ctx, &i, 8);
		SHA1_Final(this->array + 20 * (i - this->start), &ctx);
	}
}
