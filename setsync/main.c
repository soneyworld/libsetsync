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
	SET_CONFIG c = set_create_config();
	result = set_init(&s, c);
	result = set_insert_string(&s, "test");
	if(!result) {
		set_free(&s);
		return result;
	}
	result = set_find_string(&s,"test");
	if(!result) {
		set_free(&s);
		return result;
	}
	result = set_free(&s);
	printf("bla\n");
	return result;
}

