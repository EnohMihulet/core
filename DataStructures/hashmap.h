#pragma once

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

/*
	NOTE:
	#define CORE_HASHMAP_IMPLEMENTATION
	before you include this file in only one C or C++ file

	NOTE:
	Generic hash map. The hash function returns a stable hash for a key.
	The equals function returns true when two keys are equal.

HASHMAP_DEFINE(PREFIX, KEY_TYPE, VALUE_TYPE, INIT_CAPACITY) generates:

typedef struct {
	KEY_TYPE key;
	VALUE_TYPE value;
	uint64_t hash;
	bool occupied;
	bool deleted;
} PREFIX##_HashMapEntry;

typedef struct {
	size_t size, capacity;
	uint64_t (*hash)(const KEY_TYPE* key);
	bool (*equals)(const KEY_TYPE* x, const KEY_TYPE* y);
	PREFIX##_HashMapEntry* entries;
} PREFIX##_HashMap;

bool        PREFIX##_init(PREFIX##_HashMap* map, uint64_t (*hash)(const KEY_TYPE* key), bool (*equals)(const KEY_TYPE* x, const KEY_TYPE* y));
void        PREFIX##_free(PREFIX##_HashMap* map);
void        PREFIX##_clear(PREFIX##_HashMap* map);
bool        PREFIX##_reserve(PREFIX##_HashMap* map, size_t capacity);
bool        PREFIX##_insert(PREFIX##_HashMap* map, KEY_TYPE key, VALUE_TYPE value);
bool        PREFIX##_set(PREFIX##_HashMap* map, KEY_TYPE key, VALUE_TYPE value);
bool        PREFIX##_remove(PREFIX##_HashMap* map, KEY_TYPE key);
bool        PREFIX##_contains(const PREFIX##_HashMap* map, KEY_TYPE key);
bool        PREFIX##_get(const PREFIX##_HashMap* map, KEY_TYPE key, VALUE_TYPE* out);
VALUE_TYPE* PREFIX##_get_ptr(PREFIX##_HashMap* map, KEY_TYPE key);
bool        PREFIX##_is_empty(const PREFIX##_HashMap* map);
float       PREFIX##_load_factor(const PREFIX##_HashMap* map);
*/

#ifdef CORE_HASHMAP_IMPLEMENTATION
	#define CORE_HASHMAP_IMPL(...) __VA_ARGS__
#else
	#define CORE_HASHMAP_IMPL(...)
#endif

#define HASHMAP_DEFINE(PREFIX, KEY_TYPE, VALUE_TYPE, INIT_CAPACITY)                    \
	typedef struct {                                                                 \
		KEY_TYPE key;                                                              \
		VALUE_TYPE value;                                                          \
		uint64_t hash;                                                             \
		bool occupied;                                                             \
		bool deleted;                                                              \
	} PREFIX##_HashMapEntry;                                                         \
	typedef struct {                                                                 \
		size_t size, capacity;                                                     \
		uint64_t (*hash)(const KEY_TYPE* key);                                     \
		bool (*equals)(const KEY_TYPE* x, const KEY_TYPE* y);                      \
		PREFIX##_HashMapEntry* entries;                                            \
	} PREFIX##_HashMap;                                                              \
	bool        PREFIX##_init(PREFIX##_HashMap* map, uint64_t (*hash)(const KEY_TYPE* key), bool (*equals)(const KEY_TYPE* x, const KEY_TYPE* y)); \
	void        PREFIX##_free(PREFIX##_HashMap* map);                               \
	void        PREFIX##_clear(PREFIX##_HashMap* map);                              \
	bool        PREFIX##_reserve(PREFIX##_HashMap* map, size_t capacity);           \
	bool        PREFIX##_insert(PREFIX##_HashMap* map, KEY_TYPE key, VALUE_TYPE value); \
	bool        PREFIX##_set(PREFIX##_HashMap* map, KEY_TYPE key, VALUE_TYPE value); \
	bool        PREFIX##_remove(PREFIX##_HashMap* map, KEY_TYPE key);               \
	bool        PREFIX##_contains(const PREFIX##_HashMap* map, KEY_TYPE key);        \
	bool        PREFIX##_get(const PREFIX##_HashMap* map, KEY_TYPE key, VALUE_TYPE* out); \
	VALUE_TYPE* PREFIX##_get_ptr(PREFIX##_HashMap* map, KEY_TYPE key);              \
	bool        PREFIX##_is_empty(const PREFIX##_HashMap* map);                     \
	float       PREFIX##_load_factor(const PREFIX##_HashMap* map);                  \
	CORE_HASHMAP_IMPL()
