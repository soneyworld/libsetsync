/*
 * main.c
 *
 *  Created on: 01.08.2012
 *      Author: Till Lorentzen
 */

#include "set.h"

#include <stdio.h>

int main(int argc, const char* argv[]) {
	int result = 0;
	SET s;
	SET_CONFIG c;
	c.function = SHA_1;
	result = set_init(&s, c);
	result = set_free(&s);
	return result;
}

