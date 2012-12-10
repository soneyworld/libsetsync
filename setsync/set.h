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
#include "config.h"
#include <unistd.h>
#include <string.h>
#include <inttypes.h>

typedef struct {
	void *set;
	void *error;
} SET;

typedef struct {
	void * process;
	void * error;
} SET_SYNC_HANDLE;

typedef enum {
	MD_2, MD_4, MD_5, SHA_1, SHA_224, SHA_256, SHA_384, SHA_512
} SET_HASH_FUNCTION;

typedef enum {
	LEVELDB, BERKELEY_DB
} SET_STORAGE_TYPE;

typedef struct {
	SET_HASH_FUNCTION function;
	SET_STORAGE_TYPE storage;
	int bf_hard_max;
	uint64_t bf_max_elements;
	float false_positive_rate;
} SET_CONFIG;

typedef void diff_callback(void *closure, const unsigned char * hash,
		const size_t hashsize, const size_t existsLocally);

// Set configuration
#ifdef HAVE_IBRCOMMON
int set_load_config_file(SET_CONFIG * config);
#endif
SET_CONFIG set_create_config();

// Init and Free
int set_init(SET *set, SET_CONFIG config);
int set_init_with_path(SET *set, SET_CONFIG config, const char * path);
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
int set_insert_data(SET *set, const void * data, const size_t length);

int set_erase(SET *set, const unsigned char * key);
int set_erase_string(SET *set, const char * str);
int set_clear(SET *set);

// Synchronization
size_t set_sync_calc_output_buffer_size(SET_SYNC_HANDLE * handle,
		const size_t RTT, const size_t bandwidth);
int set_sync_init_handle(SET * set, SET_SYNC_HANDLE * handle);
ssize_t set_sync_step(SET_SYNC_HANDLE * handle, void * inbuf,
		const size_t inlength, void * outbuf, const size_t maxoutlength,
		diff_callback * callback, void * closure);
int set_sync_done(SET_SYNC_HANDLE * handle);
int set_sync_free_handle(SET_SYNC_HANDLE * handle);

// Error Handling
/**
 * \return the last error message
 */
const char * set_last_error_to_string(SET * set);
/**
 * Prints the last error message to stdout
 * \return 0 on success
 */
int set_last_error_printf(SET * set);

#ifdef __cplusplus
}
#endif

#endif /* SET_H_ */
