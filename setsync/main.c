/*
 * main.c
 *
 *  Created on: 01.08.2012
 *      Author: Till Lorentzen
 */

#include "set.h"
#include "bloom/bloom.h"

#include <stdio.h>

int main( int argc, const char* argv[] )
{
	size_t m = 0;
	size_t k = 0;
	int result = bloom_calc_parameter(100,0.001,&m,&k);
	printf("m=%d k=%d\n", (int)m,(int)k);
	BLOOM *b = bloom_create(m,k);
	result = bloom_destroy(b);
	SET s;
	set_init(&s);
	set_free(&s);
	return result;
}

