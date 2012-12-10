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
	c.storage = LEVELDB;
	result = set_init(&s, c);
	result = set_insert_string(&s, "test");
	if (!result) {
		printf("adding \"test\" failed\n");
		set_free(&s);
		return result;
	}
	result = set_find_string(&s, "test");
	if (!result) {
		printf("finding \"test\" failed\n");
		set_free(&s);
		return result;
	}
	result = set_free(&s);
	if (result != 0) {
		printf("error on deleting set1: %d\n", result);
		return result;
	}
	SET s2;
	SET_CONFIG c2 = set_create_config();
	c2.storage = BERKELEY_DB;
	result = set_init(&s2, c2);
	result = set_insert_string(&s2, "test");
	if (!result) {
		printf("adding \"test\" failed\n");
		set_free(&s2);
		return result;
	}
	result = set_find_string(&s2, "test");
	if (!result) {
		printf("finding \"test\" failed\n");
		set_free(&s2);
		return result;
	}
	result = set_free(&s2);
	if (result != 0) {
		printf("error on deleting set2: %d\n", result);
	}
	return result;
}

