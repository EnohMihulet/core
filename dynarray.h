#pragma once

#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

/*
	NOTE:
	#define CORE_DYNARRAY_IMPLEMENTATION
	before you include this file in only one C or C++ file

	NOTE:
	Safe only for plain old data / trivially copyable types.
	Not suitable for C++ types with constructors, destructors, or invariants.

DYNARRAY_DEFINE(PREFIX, TYPE, INIT_CAPACITY) generates:

typedef struct {
	size_t size, capacity;
	TYPE* items;
} PREFIX##_DynArray;

bool   PREFIX##_init(PREFIX##_DynArray* darr);
void   PREFIX##_free(PREFIX##_DynArray* darr);
bool   PREFIX##_is_full(const PREFIX##_DynArray* darr);
bool   PREFIX##_is_empty(const PREFIX##_DynArray* darr);
void   PREFIX##_clear(PREFIX##_DynArray* darr);
bool   PREFIX##_push(PREFIX##_DynArray* darr, TYPE item);
bool   PREFIX##_pop(PREFIX##_DynArray* darr, TYPE* out);
bool   PREFIX##_insert_at(PREFIX##_DynArray* darr, size_t index, TYPE item);
bool   PREFIX##_at(const PREFIX##_DynArray* darr, size_t index, TYPE* out);
size_t PREFIX##_find_index(const PREFIX##_DynArray* darr, int (*cmp)(const TYPE* x, const TYPE* y), TYPE item); 
bool   PREFIX##_find(const PREFIX##_DynArray* darr, int (*cmp)(const TYPE* x, const TYPE* y), TYPE item, TYPE* out);
void   PREFIX##_sort(PREFIX##_DynArray* darr, int (*cmp)(const TYPE* x, const TYPE* y));


cmp contract:
- cmp(&x, &y) < 0  => x comes before y
- cmp(&x, &y) == 0 => x and y compare equal
- cmp(&x, &y) > 0  => x comes after y

PREFIX##_find_index and PREFIX##_find treat cmp(&item, &element) == 0 as a match.
PREFIX##_find_index returns darr->size as "not found".
PREFIX##_sort expects cmp to define a consistent ordering.
*/

#define CORE_DYNARRAY_GROWTH_RATE 2
#define CORE_DYNARRAY_DEFAULT_CAPACITY 8

#ifdef CORE_DYNARRAY_IMPLEMENTATION
	#define CORE_DYNARRAY_IMPL(...) __VA_ARGS__
#else
	#define CORE_DYNARRAY_IMPL(...)
#endif

#define DYNARRAY_DEFINE(PREFIX, TYPE, INIT_CAPACITY)                                                                                                \
                                                                                                                                                    \
	typedef struct {                                                                                                                            \
		size_t size, capacity;                                                                                                              \
		TYPE* items;                                                                                                                        \
	} PREFIX##_DynArray;                                                                                                                        \
                                                                                                                                                    \
	bool   PREFIX##_init(PREFIX##_DynArray* darr);                                                                                              \
	void   PREFIX##_free(PREFIX##_DynArray* darr);                                                                                              \
	bool   PREFIX##_is_full(const PREFIX##_DynArray* darr);                                                                                     \
	bool   PREFIX##_is_empty(const PREFIX##_DynArray* darr);                                                                                    \
	void   PREFIX##_clear(PREFIX##_DynArray* darr);                                                                                             \
	bool   PREFIX##_push(PREFIX##_DynArray* darr, TYPE item);                                                                                   \
	bool   PREFIX##_pop(PREFIX##_DynArray* darr, TYPE* out);                                                                                    \
	bool   PREFIX##_insert_at(PREFIX##_DynArray* darr, size_t index, TYPE item);                                                                \
	bool   PREFIX##_at(const PREFIX##_DynArray* darr, size_t index, TYPE* out);                                                                 \
	size_t PREFIX##_find_index(const PREFIX##_DynArray* darr, int (*cmp)(const TYPE* x, const TYPE* y), TYPE item);                             \
	bool   PREFIX##_find(const PREFIX##_DynArray* darr, int (*cmp)(const TYPE* x, const TYPE* y), TYPE item, TYPE* out);                        \
	void   PREFIX##_sort(PREFIX##_DynArray* darr, int (*cmp)(const TYPE* x, const TYPE* y));                                                    \
                                                                                                                                                    \
	CORE_DYNARRAY_IMPL(                                                                                                                         \
		static bool PREFIX##_grow_impl(PREFIX##_DynArray* darr) {                                                                           \
			assert(darr != NULL);                                                                                                       \
                                                                                                                                                    \
			size_t new_capacity = darr->capacity ? darr->capacity * CORE_DYNARRAY_GROWTH_RATE : CORE_DYNARRAY_DEFAULT_CAPACITY;         \
			TYPE* new_items = (TYPE*)realloc(darr->items, new_capacity * sizeof(TYPE));                                                 \
			if (new_items == NULL) return false;                                                                                        \
                                                                                                                                                    \
			darr->items = new_items;                                                                                                    \
			darr->capacity = new_capacity;                                                                                              \
			return true;                                                                                                                \
		}                                                                                                                                   \
                                                                                                                                                    \
		static void PREFIX##_swap_impl(PREFIX##_DynArray* darr, size_t i, size_t j) {                                                       \
			assert(darr != NULL);                                                                                                       \
			TYPE temp = darr->items[i];                                                                                                 \
			darr->items[i] = darr->items[j];                                                                                            \
			darr->items[j] = temp;                                                                                                      \
		}                                                                                                                                   \
                                                                                                                                                    \
		static size_t PREFIX##_partition_impl(PREFIX##_DynArray* darr, int (*cmp)(const TYPE* x, const TYPE* y), size_t low, size_t high) { \
			assert(darr != NULL);                                                                                                       \
			assert(cmp != NULL);                                                                                                        \
                                                                                                                                                    \
			TYPE pivot = darr->items[high];                                                                                             \
			size_t j = low;                                                                                                             \
                                                                                                                                                    \
			for (size_t i = low; i < high; i++) {                                                                                       \
				TYPE item = darr->items[i];                                                                                         \
				if (cmp(&item, &pivot) <= 0) {                                                                                      \
					PREFIX##_swap_impl(darr, i, j);                                                                             \
					j++;                                                                                                        \
				}                                                                                                                   \
			}                                                                                                                           \
                                                                                                                                                    \
			PREFIX##_swap_impl(darr, j, high);                                                                                          \
			return j;                                                                                                                   \
		}                                                                                                                                   \
                                                                                                                                                    \
		static void PREFIX##_quicksort_impl(PREFIX##_DynArray* darr, int (*cmp)(const TYPE* x, const TYPE* y), size_t low, size_t high) {   \
			if (low < high) {                                                                                                           \
				size_t i = PREFIX##_partition_impl(darr, cmp, low, high);                                                           \
				if (i > 0) PREFIX##_quicksort_impl(darr, cmp, low, i - 1);                                                          \
				PREFIX##_quicksort_impl(darr, cmp, i + 1, high);                                                                    \
			}                                                                                                                           \
		}                                                                                                                                   \
                                                                                                                                                    \
		bool PREFIX##_init(PREFIX##_DynArray* darr) {                                                                                       \
			assert(darr != NULL);                                                                                                       \
			size_t cap = (INIT_CAPACITY) == 0 ? CORE_DYNARRAY_DEFAULT_CAPACITY : (INIT_CAPACITY);                                       \
			darr->items = (TYPE*)malloc(cap * sizeof(TYPE));                                                                            \
			if (darr->items == NULL) return false;                                                                                      \
			darr->size = 0;                                                                                                             \
			darr->capacity = cap;                                                                                                       \
			return true;                                                                                                                \
		}                                                                                                                                   \
                                                                                                                                                    \
		void PREFIX##_free(PREFIX##_DynArray* darr) {                                                                                       \
			assert(darr != NULL);                                                                                                       \
			free(darr->items);                                                                                                          \
			darr->items = NULL;                                                                                                         \
			darr->size = 0;                                                                                                             \
			darr->capacity = 0;                                                                                                         \
		}                                                                                                                                   \
                                                                                                                                                    \
		bool PREFIX##_is_full(const PREFIX##_DynArray* darr) {                                                                              \
			assert(darr != NULL);                                                                                                       \
			return darr->size >= darr->capacity;                                                                                        \
		}                                                                                                                                   \
                                                                                                                                                    \
		bool PREFIX##_is_empty(const PREFIX##_DynArray* darr) {                                                                             \
			assert(darr != NULL);                                                                                                       \
			return darr->size == 0;                                                                                                     \
		}                                                                                                                                   \
                                                                                                                                                    \
		void PREFIX##_clear(PREFIX##_DynArray* darr) {                                                                                      \
			assert(darr != NULL);                                                                                                       \
			darr->size = 0;                                                                                                             \
		}                                                                                                                                   \
                                                                                                                                                    \
		bool PREFIX##_push(PREFIX##_DynArray* darr, TYPE item) {                                                                            \
			assert(darr != NULL);                                                                                                       \
			if (PREFIX##_is_full(darr)) {                                                                                               \
				if (!PREFIX##_grow_impl(darr)) return false;                                                                        \
			}                                                                                                                           \
			darr->items[darr->size++] = item;                                                                                           \
			return true;                                                                                                                \
		}                                                                                                                                   \
                                                                                                                                                    \
		bool PREFIX##_pop(PREFIX##_DynArray* darr, TYPE* out) {                                                                             \
			assert(darr != NULL);                                                                                                       \
			assert(out != NULL);                                                                                                        \
			if (PREFIX##_is_empty(darr)) return false;                                                                                  \
			*out = darr->items[--darr->size];                                                                                           \
			return true;                                                                                                                \
		}                                                                                                                                   \
                                                                                                                                                    \
		bool PREFIX##_insert_at(PREFIX##_DynArray* darr, size_t index, TYPE item) {                                                         \
			assert(darr != NULL);                                                                                                       \
			if (index > darr->size) return false;                                                                                       \
			if (PREFIX##_is_full(darr)) {                                                                                               \
				if (!PREFIX##_grow_impl(darr)) return false;                                                                        \
			}                                                                                                                           \
			for (size_t i = darr->size; i > index; i--) {                                                                               \
				darr->items[i] = darr->items[i-1];                                                                                  \
			}                                                                                                                           \
			darr->items[index] = item;                                                                                                  \
			darr->size++;                                                                                                               \
			return true;                                                                                                                \
		}                                                                                                                                   \
                                                                                                                                                    \
		bool PREFIX##_at(const PREFIX##_DynArray* darr, size_t index, TYPE* out) {                                                          \
			assert(darr != NULL);                                                                                                       \
			assert(out != NULL);                                                                                                        \
			if (index >= darr->size) return false;                                                                                      \
			*out = darr->items[index];                                                                                                  \
			return true;                                                                                                                \
		}                                                                                                                                   \
                                                                                                                                                    \
		size_t PREFIX##_find_index(const PREFIX##_DynArray* darr, int (*cmp)(const TYPE* x, const TYPE* y), TYPE item) {                    \
			assert(darr != NULL);                                                                                                       \
			assert(cmp != NULL);                                                                                                        \
			for (size_t i = 0; i < darr->size; i++) {                                                                                   \
				if (cmp(&item, &darr->items[i]) == 0) return i;                                                                     \
			}                                                                                                                           \
			return darr->size;                                                                                                          \
		}                                                                                                                                   \
                                                                                                                                                    \
		bool PREFIX##_find(const PREFIX##_DynArray* darr, int (*cmp)(const TYPE* x, const TYPE* y), TYPE item, TYPE* out) {                 \
			assert(darr != NULL);                                                                                                       \
			assert(out != NULL);                                                                                                        \
			assert(cmp != NULL);                                                                                                        \
			for (size_t i = 0; i < darr->size; i++) {                                                                                   \
				if (cmp(&item, &darr->items[i]) == 0) {                                                                             \
					*out = darr->items[i];                                                                                      \
					return true;                                                                                                \
				}                                                                                                                   \
			}                                                                                                                           \
			return false;                                                                                                               \
		}                                                                                                                                   \
                                                                                                                                                    \
		void PREFIX##_sort(PREFIX##_DynArray* darr, int (*cmp)(const TYPE* x, const TYPE* y)) {                                             \
			assert(darr != NULL);                                                                                                       \
			assert(cmp != NULL);                                                                                                        \
			if (darr->size == 0) return;                                                                                                \
			PREFIX##_quicksort_impl(darr, cmp, 0, darr->size - 1);                                                                      \
		}                                                                                                                                   \
	)
