/*
 * main.c
 *
 *  Created on: 01.08.2012
 *      Author: Till Lorentzen
 */
#define SET_SIZE 10000
#include "set.h"

#include <stdio.h>

int main(int argc, const char* argv[]) {
	int result = 0;
	SET s;
	SET_CONFIG c = set_create_config();
	c.storage = BERKELEY_DB;
	c.storage_cache_bytes = 20 * 1024;
	result = set_init(&s, c);
	if (result != 0) {
		printf("error on init berkeley db set: %s\n",
				set_last_error_to_string(&s));
		return -1;
	}
	unsigned int i;
	size_t n;
	char buffer[50];
	for (i = 0; i < SET_SIZE; i++) {
		n = sprintf(buffer, "test %d", i);
		result = set_insert_string(&s, buffer);
		if (!result) {
			printf("adding %s failed\n",buffer);
			set_free(&s);
			return result;
		}
	}
/*	result = set_find_string(&s, "test");
	if (!result) {
		printf("finding \"test\" failed\n");
		set_free(&s);
		return result;
	} */
	result = set_free(&s);
	if (result != 0) {
		printf("error on deleting set1: %d\n", result);
		return result;
	}
	/*
	SET s2;
	SET_CONFIG c2 = set_create_config();
	c2.storage = LEVELDB;
	result = set_init(&s2, c2);
	if (result != 0) {
		printf("error on init level db set: %s\n",
				set_last_error_to_string(&s2));
		return -1;
	}
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
	SET_SYNC_HANDLE handle;
	set_sync_init_handle(&s2, &handle);
	printf("%ld\n", set_sync_calc_output_buffer_size(&handle, 1000, 1000));
	set_sync_free_handle(&handle);
	result = set_free(&s2);
	if (result != 0) {
		printf("error on deleting set2: %d\n", result);
	}*/
	return result;

}

