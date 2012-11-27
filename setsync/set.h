/*
 * set.h
 *
 *  Created on: 12.07.2012
 *      Author: Till Lorentzen
 */

#ifndef SET_H_
#define SET_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>

typedef struct {
	void *set;
	void *hash;
} SET;

typedef void diff_callback(void *closure, const unsigned char * hash,
		const size_t hashsize, const size_t existsLocally);

// Init and Free
int set_init(SET *set, const char * path, const size_t maxSize,
		const int hardMaximum, const float falsePositiveRate);
int set_init_with_path(SET *set, const char * path);
int set_free(SET *set);

// Capacity
int set_empty(SET *set);
size_t set_size(SET *set);
size_t set_max_size(SET *set);

// Lookup
int set_find(SET *set, const unsigned char * key);
int set_find_string(SET *set, const char * str);

// Modifiers
int set_insert(SET *set, const unsigned char * key);
int set_insert_string(SET *set, const char * str);
int set_erase(SET *set, const unsigned char * key);
int set_erase_string(SET *set, const char * str);
//int set_swap(SET *set);
int set_clear(SET *set);

// Iterators
struct set_iterator {

};
struct set_iterator * set_iterator_begin();
struct set_iterator * set_iterator_end();

#ifdef __cplusplus
}
#endif

#endif /* SET_H_ */
