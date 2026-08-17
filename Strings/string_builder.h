#pragma once

#include <stddef.h>
#include <stdbool.h>

/*
	NOTE:
	#define CORE_STRING_BUILDER_IMPLEMENTATION
	before you include this file in only one C or C++ file

	NOTE:
	Growable string builder for null-terminated byte strings.

STRING_BUILDER_DEFINE(PREFIX) generates:

typedef struct {
	size_t size, capacity;
	char* items;
} PREFIX##_StringBuilder;

bool        PREFIX##_init(PREFIX##_StringBuilder* sb);
void        PREFIX##_free(PREFIX##_StringBuilder* sb);
void        PREFIX##_clear(PREFIX##_StringBuilder* sb);
bool        PREFIX##_reserve(PREFIX##_StringBuilder* sb, size_t capacity);
bool        PREFIX##_append_char(PREFIX##_StringBuilder* sb, char c);
bool        PREFIX##_append_cstr(PREFIX##_StringBuilder* sb, const char* str);
bool        PREFIX##_append_slice(PREFIX##_StringBuilder* sb, const char* str, size_t len);
bool        PREFIX##_append_format(PREFIX##_StringBuilder* sb, const char* fmt, ...);
bool        PREFIX##_insert(PREFIX##_StringBuilder* sb, size_t index, const char* str);
bool        PREFIX##_erase(PREFIX##_StringBuilder* sb, size_t index, size_t len);
const char* PREFIX##_cstr(const PREFIX##_StringBuilder* sb);
char*       PREFIX##_take(PREFIX##_StringBuilder* sb);
bool        PREFIX##_is_empty(const PREFIX##_StringBuilder* sb);
*/

#ifdef CORE_STRING_BUILDER_IMPLEMENTATION
	#define CORE_STRING_BUILDER_IMPL(...) __VA_ARGS__
#else
	#define CORE_STRING_BUILDER_IMPL(...)
#endif

#define STRING_BUILDER_DEFINE(PREFIX)                                                 \
	typedef struct {                                                                 \
		size_t size, capacity;                                                     \
		char* items;                                                               \
	} PREFIX##_StringBuilder;                                                       \
	bool        PREFIX##_init(PREFIX##_StringBuilder* sb);                          \
	void        PREFIX##_free(PREFIX##_StringBuilder* sb);                          \
	void        PREFIX##_clear(PREFIX##_StringBuilder* sb);                         \
	bool        PREFIX##_reserve(PREFIX##_StringBuilder* sb, size_t capacity);      \
	bool        PREFIX##_append_char(PREFIX##_StringBuilder* sb, char c);           \
	bool        PREFIX##_append_cstr(PREFIX##_StringBuilder* sb, const char* str);  \
	bool        PREFIX##_append_slice(PREFIX##_StringBuilder* sb, const char* str, size_t len); \
	bool        PREFIX##_append_format(PREFIX##_StringBuilder* sb, const char* fmt, ...); \
	bool        PREFIX##_insert(PREFIX##_StringBuilder* sb, size_t index, const char* str); \
	bool        PREFIX##_erase(PREFIX##_StringBuilder* sb, size_t index, size_t len); \
	const char* PREFIX##_cstr(const PREFIX##_StringBuilder* sb);                    \
	char*       PREFIX##_take(PREFIX##_StringBuilder* sb);                          \
	bool        PREFIX##_is_empty(const PREFIX##_StringBuilder* sb);                \
	CORE_STRING_BUILDER_IMPL()
