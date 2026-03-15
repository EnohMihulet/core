#pragma once

#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

/*
	NOTE:
	#define CORE_STACK_IMPLEMENTATION
	before you include this file in only one C or C++ file

	NOTE:
	Safe only for plain old data / trivially copyable types.
	Not suitable for C++ types with constructors, destructors, or invariants.

STACK_DEFINE(PREFIX, TYPE, INIT_CAPACITY) generates:

typedef struct {
	size_t size, capacity;
	TYPE* items;
} PREFIX##_Stack;

bool PREFIX##_init(PREFIX##_Stack* st);
void PREFIX##_free(PREFIX##_Stack* st);
bool PREFIX##_is_full(const PREFIX##_Stack* st);
bool PREFIX##_is_empty(const PREFIX##_Stack* st);
void PREFIX##_clear(PREFIX##_Stack* st);
bool PREFIX##_push(PREFIX##_Stack* st, TYPE item);
bool PREFIX##_pop(PREFIX##_Stack* st, TYPE* out);
bool PREFIX##_top(const PREFIX##_Stack* st, TYPE* out)
*/

#define CORE_STACK_GROWTH_RATE 2
#define CORE_STACK_DEFAULT_CAPACITY 8

#ifdef CORE_STACK_IMPLEMENTATION
	#define CORE_STACK_IMPL(...) __VA_ARGS__
#else
	#define CORE_STACK_IMPL(...)
#endif

#define STACK_DEFINE(PREFIX, TYPE, INIT_CAPACITY)                                                          \
                                                                                                           \
	typedef struct {                                                                                   \
		size_t size, capacity;                                                                     \
		TYPE* items;                                                                               \
	} PREFIX##_Stack;                                                                                  \
                                                                                                           \
	bool PREFIX##_init(PREFIX##_Stack* st);                                                            \
	void PREFIX##_free(PREFIX##_Stack* st);                                                            \
	bool PREFIX##_is_full(const PREFIX##_Stack* st);                                                   \
	bool PREFIX##_is_empty(const PREFIX##_Stack* st);                                                  \
	void PREFIX##_clear(PREFIX##_Stack* st);                                                           \
	bool PREFIX##_push(PREFIX##_Stack* st, TYPE item);                                                 \
	bool PREFIX##_pop(PREFIX##_Stack* st, TYPE* out);                                                  \
	bool PREFIX##_top(const PREFIX##_Stack* st, TYPE* out);                                            \
                                                                                                           \
	CORE_STACK_IMPL(                                                                                   \
		static bool PREFIX##_grow_impl(PREFIX##_Stack* st) {                                       \
			assert(st != NULL);                                                                \
                                                                                                           \
			size_t new_capacity;                                                               \
			if (st->capacity == 0) {                                                           \
				new_capacity = CORE_STACK_DEFAULT_CAPACITY;                                \
			} else {                                                                           \
				if (CORE_STACK_GROWTH_RATE <= 1) return false;                             \
				if (st->capacity > ((size_t)-1) / CORE_STACK_GROWTH_RATE) return false;    \
				new_capacity = st->capacity * CORE_STACK_GROWTH_RATE;                      \
				if (new_capacity <= st->capacity) return false;                            \
			}                                                                                  \
			if (new_capacity > ((size_t)-1) / sizeof(TYPE)) return false;                      \
			TYPE* new_items = (TYPE*)realloc(st->items, new_capacity * sizeof(TYPE));          \
			if (new_items == NULL) return false;                                               \
                                                                                                           \
			st->items = new_items;                                                             \
			st->capacity = new_capacity;                                                       \
			return true;                                                                       \
		}                                                                                          \
                                                                                                           \
		bool PREFIX##_init(PREFIX##_Stack* st) {                                                   \
			assert(st != NULL);                                                                \
			size_t cap = (INIT_CAPACITY) == 0 ? CORE_STACK_DEFAULT_CAPACITY : (INIT_CAPACITY); \
			if (cap > ((size_t)-1) / sizeof(TYPE)) return false;                               \
			st->items = (TYPE*)malloc(cap * sizeof(TYPE));                                     \
			if (st->items == NULL) return false;                                               \
			st->size = 0;                                                                      \
			st->capacity = cap;                                                                \
			return true;                                                                       \
		}                                                                                          \
                                                                                                           \
		void PREFIX##_free(PREFIX##_Stack* st) {                                                   \
			assert(st != NULL);                                                                \
			free(st->items);                                                                   \
			st->items = NULL;                                                                  \
			st->size = 0;                                                                      \
			st->capacity = 0;                                                                  \
		}                                                                                          \
                                                                                                           \
		bool PREFIX##_is_full(const PREFIX##_Stack* st) {                                          \
			assert(st != NULL);                                                                \
			return st->size >= st->capacity;                                                   \
		}                                                                                          \
                                                                                                           \
		bool PREFIX##_is_empty(const PREFIX##_Stack* st) {                                         \
			assert(st != NULL);                                                                \
			return st->size == 0;                                                              \
		}                                                                                          \
                                                                                                           \
		void PREFIX##_clear(PREFIX##_Stack* st) {                                                  \
			assert(st != NULL);                                                                \
			st->size = 0;                                                                      \
		}                                                                                          \
                                                                                                           \
		bool PREFIX##_push(PREFIX##_Stack* st, TYPE item) {                                        \
			assert(st != NULL);                                                                \
			if (PREFIX##_is_full(st)) {                                                        \
				if (!PREFIX##_grow_impl(st)) return false;                                 \
			}                                                                                  \
			st->items[st->size++] = item;                                                      \
			return true;                                                                       \
		}                                                                                          \
                                                                                                           \
		bool PREFIX##_pop(PREFIX##_Stack* st, TYPE* out) {                                         \
			assert(st != NULL);                                                                \
			assert(out != NULL);                                                               \
			if (PREFIX##_is_empty(st)) return false;                                           \
			*out = st->items[--st->size];                                                      \
			return true;                                                                       \
		}                                                                                          \
                                                                                                           \
		bool PREFIX##_top(const PREFIX##_Stack* st, TYPE* out) {                                   \
			assert(st != NULL);                                                                \
			assert(out != NULL);                                                               \
			if (PREFIX##_is_empty(st)) return false;                                           \
			*out = st->items[st->size - 1];                                                    \
			return true;                                                                       \
		}                                                                                          \
	)
