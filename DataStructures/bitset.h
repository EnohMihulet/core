#pragma once

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

/*
	NOTE:
	#define CORE_BITSET_IMPLEMENTATION
	before you include this file in only one C or C++ file

	NOTE:
	Dynamic bitset backed by machine words.

BITSET_DEFINE(PREFIX, INIT_BITS) generates:

typedef struct {
	size_t bit_count, word_count;
	uint64_t* words;
} PREFIX##_BitSet;

bool   PREFIX##_init(PREFIX##_BitSet* bs);
void   PREFIX##_free(PREFIX##_BitSet* bs);
void   PREFIX##_clear(PREFIX##_BitSet* bs);
bool   PREFIX##_resize(PREFIX##_BitSet* bs, size_t bit_count);
bool   PREFIX##_set(PREFIX##_BitSet* bs, size_t index);
bool   PREFIX##_unset(PREFIX##_BitSet* bs, size_t index);
bool   PREFIX##_toggle(PREFIX##_BitSet* bs, size_t index);
bool   PREFIX##_assign(PREFIX##_BitSet* bs, size_t index, bool value);
bool   PREFIX##_get(const PREFIX##_BitSet* bs, size_t index, bool* out);
bool   PREFIX##_set_all(PREFIX##_BitSet* bs);
bool   PREFIX##_unset_all(PREFIX##_BitSet* bs);
size_t PREFIX##_count(const PREFIX##_BitSet* bs);
bool   PREFIX##_any(const PREFIX##_BitSet* bs);
bool   PREFIX##_all(const PREFIX##_BitSet* bs);
bool   PREFIX##_none(const PREFIX##_BitSet* bs);
*/

#ifdef CORE_BITSET_IMPLEMENTATION
	#define CORE_BITSET_IMPL(...) __VA_ARGS__
#else
	#define CORE_BITSET_IMPL(...)
#endif

#define BITSET_DEFINE(PREFIX, INIT_BITS)                                               \
	typedef struct {                                                                  \
		size_t bit_count, word_count;                                               \
		uint64_t* words;                                                            \
	} PREFIX##_BitSet;                                                                \
	bool   PREFIX##_init(PREFIX##_BitSet* bs);                                         \
	void   PREFIX##_free(PREFIX##_BitSet* bs);                                         \
	void   PREFIX##_clear(PREFIX##_BitSet* bs);                                        \
	bool   PREFIX##_resize(PREFIX##_BitSet* bs, size_t bit_count);                     \
	bool   PREFIX##_set(PREFIX##_BitSet* bs, size_t index);                            \
	bool   PREFIX##_unset(PREFIX##_BitSet* bs, size_t index);                          \
	bool   PREFIX##_toggle(PREFIX##_BitSet* bs, size_t index);                         \
	bool   PREFIX##_assign(PREFIX##_BitSet* bs, size_t index, bool value);             \
	bool   PREFIX##_get(const PREFIX##_BitSet* bs, size_t index, bool* out);           \
	bool   PREFIX##_set_all(PREFIX##_BitSet* bs);                                      \
	bool   PREFIX##_unset_all(PREFIX##_BitSet* bs);                                    \
	size_t PREFIX##_count(const PREFIX##_BitSet* bs);                                  \
	bool   PREFIX##_any(const PREFIX##_BitSet* bs);                                    \
	bool   PREFIX##_all(const PREFIX##_BitSet* bs);                                    \
	bool   PREFIX##_none(const PREFIX##_BitSet* bs);                                   \
	CORE_BITSET_IMPL()
