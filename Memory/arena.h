#pragma once

#include <stddef.h>
#include <stdbool.h>

/*
	NOTE:
	#define CORE_ARENA_IMPLEMENTATION
	before you include this file in only one C or C++ file

	NOTE:
	Linear arena allocator. Individual allocations are released together by reset/free.

ARENA_DEFINE(PREFIX) generates:

typedef struct {
	size_t size, capacity;
	unsigned char* buffer;
	bool owns_buffer;
} PREFIX##_Arena;

bool   PREFIX##_init(PREFIX##_Arena* arena, size_t capacity);
bool   PREFIX##_init_with_buffer(PREFIX##_Arena* arena, void* buffer, size_t capacity);
void   PREFIX##_free(PREFIX##_Arena* arena);
void   PREFIX##_reset(PREFIX##_Arena* arena);
void*  PREFIX##_alloc(PREFIX##_Arena* arena, size_t size);
void*  PREFIX##_alloc_aligned(PREFIX##_Arena* arena, size_t size, size_t alignment);
void*  PREFIX##_calloc(PREFIX##_Arena* arena, size_t count, size_t size);
char*  PREFIX##_strdup(PREFIX##_Arena* arena, const char* str);
size_t PREFIX##_remaining(const PREFIX##_Arena* arena);
bool   PREFIX##_contains(const PREFIX##_Arena* arena, const void* ptr);
*/

#ifdef CORE_ARENA_IMPLEMENTATION
	#define CORE_ARENA_IMPL(...) __VA_ARGS__
#else
	#define CORE_ARENA_IMPL(...)
#endif

#define ARENA_DEFINE(PREFIX)                                                           \
	typedef struct {                                                                 \
		size_t size, capacity;                                                     \
		unsigned char* buffer;                                                     \
		bool owns_buffer;                                                          \
	} PREFIX##_Arena;                                                                \
	bool   PREFIX##_init(PREFIX##_Arena* arena, size_t capacity);                    \
	bool   PREFIX##_init_with_buffer(PREFIX##_Arena* arena, void* buffer, size_t capacity); \
	void   PREFIX##_free(PREFIX##_Arena* arena);                                     \
	void   PREFIX##_reset(PREFIX##_Arena* arena);                                    \
	void*  PREFIX##_alloc(PREFIX##_Arena* arena, size_t size);                       \
	void*  PREFIX##_alloc_aligned(PREFIX##_Arena* arena, size_t size, size_t alignment); \
	void*  PREFIX##_calloc(PREFIX##_Arena* arena, size_t count, size_t size);        \
	char*  PREFIX##_strdup(PREFIX##_Arena* arena, const char* str);                  \
	size_t PREFIX##_remaining(const PREFIX##_Arena* arena);                          \
	bool   PREFIX##_contains(const PREFIX##_Arena* arena, const void* ptr);          \
	CORE_ARENA_IMPL()
