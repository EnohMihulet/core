#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdalign.h>
#include <assert.h>

/*
	NOTE:
	#define CORE_MEMPOOL_IMPLEMENTATION
	before you include this file in only one C file

	NOTE:
	Fixed-block allocator / object pool. All blocks have the same size.
	The pool owns a caller-provided or internally allocated contiguous buffer.

MEMPOOL_DEFINE(PREFIX, TYPE, INIT_CAPACITY) generates:

typedef struct {
	size_t size, capacity;
	void* buffer;
	void* free_list;
	bool owns_buffer;
} PREFIX##_MemPool;

bool   PREFIX##_init(PREFIX##_MemPool* pool);
bool   PREFIX##_init_with_buffer(PREFIX##_MemPool* pool, void* buffer, size_t capacity);
void   PREFIX##_free(PREFIX##_MemPool* pool);
bool   PREFIX##_is_full(const PREFIX##_MemPool* pool);
bool   PREFIX##_is_empty(const PREFIX##_MemPool* pool);
void   PREFIX##_clear(PREFIX##_MemPool* pool);
TYPE*  PREFIX##_alloc(PREFIX##_MemPool* pool);
bool   PREFIX##_release(PREFIX##_MemPool* pool, TYPE* item);
bool   PREFIX##_contains(const PREFIX##_MemPool* pool, const TYPE* item);
size_t PREFIX##_available(const PREFIX##_MemPool* pool);
*/

#ifdef CORE_MEMPOOL_IMPLEMENTATION
	#define CORE_MEMPOOL_IMPL(...) __VA_ARGS__
#else
	#define CORE_MEMPOOL_IMPL(...)
#endif

#define MEMPOOL_DEFINE(PREFIX, TYPE, INIT_CAPACITY)                                               \
                                                                                                  \
	typedef struct {                                                                          \
		size_t size, capacity;                                                            \
		void* buffer;                                                                     \
		void* free_list;                                                                  \
		bool owns_buffer;                                                                 \
	} PREFIX##_MemPool;                                                                       \
                                                                                                  \
	bool   PREFIX##_init(PREFIX##_MemPool* pool);                                             \
	bool   PREFIX##_init_with_buffer(PREFIX##_MemPool* pool, void* buffer, size_t capacity);  \
	void   PREFIX##_free(PREFIX##_MemPool* pool);                                             \
	bool   PREFIX##_is_full(const PREFIX##_MemPool* pool);                                    \
	bool   PREFIX##_is_empty(const PREFIX##_MemPool* pool);                                   \
	void   PREFIX##_clear(PREFIX##_MemPool* pool);                                            \
	TYPE*  PREFIX##_alloc(PREFIX##_MemPool* pool);                                            \
	bool   PREFIX##_release(PREFIX##_MemPool* pool, TYPE* item);                              \
	bool   PREFIX##_contains(const PREFIX##_MemPool* pool, const TYPE* item);                 \
	size_t PREFIX##_available(const PREFIX##_MemPool* pool);                                  \
	                                                                                           \
	CORE_MEMPOOL_IMPL(                                                                         \
	static_assert(sizeof(TYPE) >= sizeof(void*), "mempool block type is too small");          \
	static_assert(alignof(TYPE) >= alignof(void*), "mempool block type alignment is too small");\
	                                                                                           \
	static void PREFIX##_init_free_list_impl(PREFIX##_MemPool* pool) {                        \
		char* base = (char*)pool->buffer;                                                 \
	                                                                                           \
		for (size_t i = 0; i < pool->capacity - 1; i++) {                                 \
			void* curr = base + i * sizeof(TYPE);                                     \
			void* next = base + (i + 1) * sizeof(TYPE);                               \
			*(void**)curr = next;                                                     \
		}                                                                                 \
	                                                                                           \
		void* last = base + (pool->capacity - 1) * sizeof(TYPE);                          \
		*(void**)last = NULL;                                                             \
		pool->free_list = pool->buffer;                                                   \
	}                                                                                         \
	                                                                                           \
	static bool PREFIX##_block_is_free_impl(PREFIX##_MemPool* pool, TYPE* block) {            \
		for (void* curr = pool->free_list; curr != NULL; curr = *(void**)curr) {          \
			if (curr == (void*)block) return true;                                    \
		}                                                                                 \
		return false;                                                                     \
	}                                                                                         \
	                                                                                           \
	bool   PREFIX##_init(PREFIX##_MemPool* pool) {                                            \
		if (INIT_CAPACITY == 0) return false;                                             \
		if ((INIT_CAPACITY) > ((size_t)-1) / sizeof(TYPE)) return false;                  \
		if (pool == NULL) return false;                                                   \
		pool->size = 0;                                                                   \
		pool->capacity = INIT_CAPACITY;                                                   \
		pool->buffer = malloc((INIT_CAPACITY) * sizeof(TYPE));                            \
		if (pool->buffer == NULL) return false;                                           \
		pool->free_list = pool->buffer;                                                         \
		PREFIX##_init_free_list_impl(pool);                                               \
		pool->owns_buffer = true;                                                         \
		return true;                                                                      \
	}                                                                                         \
	                                                                                           \
	bool   PREFIX##_init_with_buffer(PREFIX##_MemPool* pool, void* buffer, size_t capacity) { \
		if (pool == NULL) return false;                                                   \
		if (buffer == NULL || capacity == 0) return false;                                \
		if (capacity > ((size_t)-1) / sizeof(TYPE)) return false;                         \
		if (((uintptr_t)buffer % alignof(TYPE)) != 0) return false;                       \
		pool->size = 0;                                                                   \
		pool->capacity = capacity;                                                        \
		pool->buffer = buffer;                                                            \
		pool->free_list = buffer;                                                         \
		PREFIX##_init_free_list_impl(pool);                                               \
		pool->owns_buffer = false;                                                        \
		return true;                                                                      \
	}                                                                                         \
                                                                                                  \
	void   PREFIX##_free(PREFIX##_MemPool* pool) {                                            \
		if (pool == NULL) return;                                                         \
		if (pool->owns_buffer) free(pool->buffer);                                        \
		pool->size = 0;                                                                   \
		pool->capacity = 0;                                                               \
		pool->buffer = NULL;                                                              \
		pool->free_list = NULL;                                                           \
		pool->owns_buffer = false;                                                        \
	}                                                                                         \
                                                                                                  \
	bool   PREFIX##_is_full(const PREFIX##_MemPool* pool) {                                   \
		if (pool == NULL) return false;                                                   \
		if (pool->size == pool->capacity) return true;                                    \
		return false;                                                                     \
	}                                                                                         \
                                                                                                  \
	bool   PREFIX##_is_empty(const PREFIX##_MemPool* pool) {                                  \
		if (pool == NULL) return false;                                                   \
		if (pool->size == 0) return true;                                                 \
		return false;                                                                     \
	}                                                                                         \
                                                                                                  \
	void   PREFIX##_clear(PREFIX##_MemPool* pool) {                                           \
		if (pool == NULL) return;                                                         \
		if (pool->buffer == NULL || pool->capacity == 0) return;                         \
		memset(pool->buffer, 0, pool->capacity * sizeof(TYPE));                           \
		pool->size = 0;                                                                   \
		PREFIX##_init_free_list_impl(pool);                                               \
	}                                                                                         \
                                                                                                  \
	TYPE*  PREFIX##_alloc(PREFIX##_MemPool* pool) {                                           \
		if (pool == NULL) return NULL;                                                    \
		if (PREFIX##_is_full(pool)) return NULL;                                          \
		TYPE* block = pool->free_list;                                                    \
		pool->free_list = *(void**)block;                                                 \
		pool->size += 1;                                                                  \
		return block;                                                                     \
	}                                                                                         \
                                                                                                  \
	bool   PREFIX##_release(PREFIX##_MemPool* pool, TYPE* item) {                             \
		if (pool == NULL) return false;                                                   \
		if (!PREFIX##_contains(pool, item)) return false;                                \
		if (PREFIX##_block_is_free_impl(pool, item)) return false;                        \
		*(void**)item = pool->free_list;                                                  \
		pool->free_list = item;                                                           \
		pool->size -= 1;                                                                  \
		return true;                                                                      \
	}                                                                                         \
                                                                                                  \
	bool   PREFIX##_contains(const PREFIX##_MemPool* pool, const TYPE* item) {                \
		if (pool == NULL) return false;                                                   \
		if (pool->buffer == NULL || pool->capacity == 0 || item == NULL) return false;    \
		uintptr_t start = (uintptr_t)pool->buffer;                                        \
		uintptr_t bytes = pool->capacity * sizeof(TYPE);                                  \
		uintptr_t end = start + bytes;                                                    \
		uintptr_t p = (uintptr_t)item;                                                     \
	                                                                                           \
		if (p < start || p >= end) return false;                                          \
		return ((p - start) % sizeof(TYPE)) == 0;                                         \
	}                                                                                         \
                                                                                                  \
	size_t PREFIX##_available(const PREFIX##_MemPool* pool) {                                 \
		if (pool == NULL) return 0;                                                       \
		return pool->capacity - pool->size;                                               \
	}                                                                                         \
	)
