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
} SET;

// Init and Free
int set_init(SET *set);
int set_free(SET *set);

// Capacity
int set_empty(SET *set);
size_t set_size(SET *set);
size_t set_max_size(SET *set);

// Lookup
int find(SET *set, const char * key);

// Modifiers
int set_insert(SET *set, const char * key);
int set_erase(SET *set, const char * key);
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
