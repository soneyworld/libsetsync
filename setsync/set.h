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
	LEVELDB, BERKELEY_DB, IN_MEMORY_DB
} SET_STORAGE_TYPE;

typedef struct {
	SET_HASH_FUNCTION function;
	SET_STORAGE_TYPE storage;
	int bf_hard_max;
	uint64_t bf_max_elements;
	float false_positive_rate;
	size_t storage_cache_gbytes;
	size_t storage_cache_bytes;
} SET_CONFIG;

typedef void diff_callback(void *closure, const unsigned char * hash,
		const size_t hashsize, const size_t existsLocally);

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
// Lookup and request data
int set_get_data(SET *set, const unsigned char * key, unsigned char ** value,
		size_t * valueSize);

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

// Equality Check Method
int set_sync_get_root_hash(SET_SYNC_HANDLE * handle, unsigned char * hash);

int set_sync_is_equal_to_hash(SET_SYNC_HANDLE * handle,
		const unsigned char * remotehash);
// Bloom Filter Synchronization
/**
 * Returns 0 if no more bf data is available, otherwise the size, which is available to be sent
 */
int set_sync_bf_output_avail(SET_SYNC_HANDLE * handle);

/**
 * Reads the next data from bloom filter into the buffer and returns the written size
 */
size_t set_sync_bf_readsome(SET_SYNC_HANDLE * handle, unsigned char* buffer,
		const size_t buffersize);
/**
 * Checks the external bloom filter input and calculates the difference
 */
int set_sync_bf_diff(SET_SYNC_HANDLE * handle, const unsigned char* inbuffer,
		const size_t inlength, diff_callback * callback, void * closure);
// Synchronization buffer
size_t set_sync_min_trie_buffer(SET_SYNC_HANDLE * handle);
size_t set_sync_min_bf_buffer(SET_SYNC_HANDLE * handle);
size_t set_sync_min_buffer(SET_SYNC_HANDLE * handle);

// Trie Synchronization
int set_sync_trie_subtrie_output_avail(SET_SYNC_HANDLE * handle);
ssize_t set_sync_trie_get_subtrie(SET_SYNC_HANDLE * handle,
		unsigned char * buffer, const size_t length);
ssize_t set_sync_trie_process_subtrie(SET_SYNC_HANDLE * handle,
		unsigned char* buffer, const size_t length);
int set_sync_trie_acks_avail(SET_SYNC_HANDLE * handle);
ssize_t set_sync_trie_read_acks(SET_SYNC_HANDLE * handle,
		unsigned char * buffer, const size_t length, size_t * numberOfAcks);
int set_sync_trie_process_acks(SET_SYNC_HANDLE * handle,
		const unsigned char * buffer, const size_t length,
		const size_t numberOfAcks, diff_callback * callback, void * closure);

// Transfered Sizes
size_t set_sync_sent_bytes(SET_SYNC_HANDLE * handle);
size_t set_sync_received_bytes(SET_SYNC_HANDLE * handle);

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
