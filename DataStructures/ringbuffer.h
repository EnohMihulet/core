#pragma once

#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

/*
	NOTE:
	#define CORE_RINGBUFFER_IMPLEMENTATION
	before you include this file in only one C or C++ file

	NOTE:
	Safe only for plain old data / trivially copyable types.
	Not suitable for C++ types with constructors, destructors, or invariants.

RINGBUFFER_DEFINE(PREFIX, TYPE, INIT_CAPACITY) generates:

typedef struct {
	size_t size, capacity, front, back;
	TYPE* items;
} PREFIX##_RingBuffer;

bool   PREFIX##_init(PREFIX##_RingBuffer* rbuf);
void   PREFIX##_free(PREFIX##_RingBuffer* rbuf);
bool   PREFIX##_is_full(const PREFIX##_RingBuffer* rbuf);
bool   PREFIX##_is_empty(const PREFIX##_RingBuffer* rbuf);
void   PREFIX##_clear(PREFIX##_RingBuffer* rbuf);
bool   PREFIX##_push_front(PREFIX##_RingBuffer* rbuf, TYPE item);
bool   PREFIX##_push_back(PREFIX##_RingBuffer* rbuf, TYPE item);
bool   PREFIX##_pop_front(PREFIX##_RingBuffer* rbuf, TYPE* out);
bool   PREFIX##_pop_back(PREFIX##_RingBuffer* rbuf, TYPE* out);
bool   PREFIX##_insert_at(PREFIX##_RingBuffer* rbuf, size_t index, TYPE item);
bool   PREFIX##_erase_at(PREFIX##_RingBuffer* rbuf, size_t index);
bool   PREFIX##_at(const PREFIX##_RingBuffer* rbuf, size_t index, TYPE* out);
size_t PREFIX##_find_index(const PREFIX##_RingBuffer* rbuf, int (*cmp)(const TYPE* x, const TYPE* y), TYPE item); 
bool   PREFIX##_find(const PREFIX##_RingBuffer* rbuf, int (*cmp)(const TYPE* x, const TYPE* y), TYPE item, TYPE* out);
void   PREFIX##_sort(PREFIX##_RingBuffer* rbuf, int (*cmp)(const TYPE* x, const TYPE* y));


cmp contract:
- cmp(&x, &y) < 0  => x comes before y
- cmp(&x, &y) == 0 => x and y compare equal
- cmp(&x, &y) > 0  => x comes after y

PREFIX##_find_index and PREFIX##_find treat cmp(&item, &element) == 0 as a match.
PREFIX##_find_index returns rbuf->size as "not found".
PREFIX##_sort expects cmp to define a consistent ordering.
*/

#define CORE_RINGBUFFER_GROWTH_RATE 2
#define CORE_RINGBUFFER_DEFAULT_CAPACITY 8

#ifdef CORE_RINGBUFFER_IMPLEMENTATION
	#define CORE_RINGBUFFER_IMPL(...) __VA_ARGS__
#else
	#define CORE_RINGBUFFER_IMPL(...)
#endif

#define RINGBUFFER_DEFINE(PREFIX, TYPE, INIT_CAPACITY)                                                                                                \
                                                                                                                                                      \
	typedef struct {                                                                                                                              \
		size_t size, capacity, front, back;                                                                                                   \
		TYPE* items;                                                                                                                          \
	} PREFIX##_RingBuffer;                                                                                                                        \
                                                                                                                                                      \
	bool   PREFIX##_init(PREFIX##_RingBuffer* rbuf);                                                                                              \
	void   PREFIX##_free(PREFIX##_RingBuffer* rbuf);                                                                                              \
	bool   PREFIX##_is_full(const PREFIX##_RingBuffer* rbuf);                                                                                     \
	bool   PREFIX##_is_empty(const PREFIX##_RingBuffer* rbuf);                                                                                    \
	void   PREFIX##_clear(PREFIX##_RingBuffer* rbuf);                                                                                             \
	bool   PREFIX##_push_front(PREFIX##_RingBuffer* rbuf, TYPE item);                                                                             \
	bool   PREFIX##_push_back(PREFIX##_RingBuffer* rbuf, TYPE item);                                                                              \
	bool   PREFIX##_pop_front(PREFIX##_RingBuffer* rbuf, TYPE* out);                                                                              \
	bool   PREFIX##_pop_back(PREFIX##_RingBuffer* rbuf, TYPE* out);                                                                               \
	bool   PREFIX##_insert_at(PREFIX##_RingBuffer* rbuf, size_t index, TYPE item);                                                                \
	bool   PREFIX##_erase_at(PREFIX##_RingBuffer* rbuf, size_t index);                                                                            \
	bool   PREFIX##_at(const PREFIX##_RingBuffer* rbuf, size_t index, TYPE* out);                                                                 \
	size_t PREFIX##_find_index(const PREFIX##_RingBuffer* rbuf, int (*cmp)(const TYPE* x, const TYPE* y), TYPE item);                             \
	bool   PREFIX##_find(const PREFIX##_RingBuffer* rbuf, int (*cmp)(const TYPE* x, const TYPE* y), TYPE item, TYPE* out);                        \
	void   PREFIX##_sort(PREFIX##_RingBuffer* rbuf, int (*cmp)(const TYPE* x, const TYPE* y));                                                    \
                                                                                                                                                      \
	CORE_RINGBUFFER_IMPL(                                                                                                                         \
		static size_t PREFIX##_index_impl(const PREFIX##_RingBuffer* rbuf, size_t i) {                                                        \
			return (rbuf->front + i) % rbuf->capacity;                                                                                    \
		}                                                                                                                                     \
                                                                                                                                                      \
		static size_t PREFIX##_inc_impl(const PREFIX##_RingBuffer* rbuf, size_t i) {                                                          \
			return (i + 1 == rbuf->capacity) ? 0 : i + 1;                                                                                 \
		}                                                                                                                                     \
                                                                                                                                                      \
		static size_t PREFIX##_dec_impl(const PREFIX##_RingBuffer* rbuf, size_t i) {                                                          \
			return (i == 0) ? rbuf->capacity - 1 : i - 1;                                                                                 \
		}                                                                                                                                     \
                                                                                                                                                      \
		static bool PREFIX##_grow_impl(PREFIX##_RingBuffer* rbuf) {                                                                           \
			assert(rbuf != NULL);                                                                                                         \
                                                                                                                                                      \
			size_t new_capacity;                                                                                                          \
			if (rbuf->capacity == 0) {                                                                                                    \
				new_capacity = CORE_RINGBUFFER_DEFAULT_CAPACITY;                                                                      \
			} else {                                                                                                                      \
				if (CORE_RINGBUFFER_GROWTH_RATE <= 1) return false;                                                                   \
				if (rbuf->capacity > ((size_t)-1) / CORE_RINGBUFFER_GROWTH_RATE) return false;                                        \
				new_capacity = rbuf->capacity * CORE_RINGBUFFER_GROWTH_RATE;                                                          \
				if (new_capacity <= rbuf->capacity) return false;                                                                     \
			}                                                                                                                             \
			if (new_capacity > ((size_t)-1) / sizeof(TYPE)) return false;                                                                 \
			TYPE* new_items = (TYPE*)malloc(new_capacity * sizeof(TYPE));                                                                 \
			if (new_items == NULL) return false;                                                                                          \
			for (size_t i = 0; i < rbuf->size; i++) {                                                                                     \
				new_items[i] = rbuf->items[PREFIX##_index_impl(rbuf, i)];                                                             \
			}                                                                                                                             \
                                                                                                                                                      \
			free(rbuf->items);                                                                                                            \
			rbuf->items = new_items;                                                                                                      \
			rbuf->front = 0;                                                                                                              \
			rbuf->back = rbuf->size ? rbuf->size - 1 : 0;                                                                                 \
			rbuf->capacity = new_capacity;                                                                                                \
			return true;                                                                                                                  \
		}                                                                                                                                     \
                                                                                                                                                      \
		static void PREFIX##_swap_impl(PREFIX##_RingBuffer* rbuf, size_t i, size_t j) {                                                       \
			assert(rbuf != NULL);                                                                                                         \
			TYPE temp = rbuf->items[i];                                                                                                   \
			rbuf->items[i] = rbuf->items[j];                                                                                              \
			rbuf->items[j] = temp;                                                                                                        \
		}                                                                                                                                     \
                                                                                                                                                      \
		static size_t PREFIX##_partition_impl(PREFIX##_RingBuffer* rbuf, int (*cmp)(const TYPE* x, const TYPE* y), size_t low, size_t high) { \
			assert(rbuf != NULL);                                                                                                         \
			assert(cmp != NULL);                                                                                                          \
                                                                                                                                                      \
			TYPE* pivot = &rbuf->items[PREFIX##_index_impl(rbuf, high)];                                                                  \
			size_t j = low;                                                                                                               \
                                                                                                                                                      \
			for (size_t i = low; i < high; i++) {                                                                                         \
				TYPE* item = &rbuf->items[PREFIX##_index_impl(rbuf, i)];                                                              \
				if (cmp(item, pivot) <= 0) {                                                                                          \
					PREFIX##_swap_impl(rbuf, PREFIX##_index_impl(rbuf, i), PREFIX##_index_impl(rbuf, j));                         \
					j++;                                                                                                          \
				}                                                                                                                     \
			}                                                                                                                             \
                                                                                                                                                      \
			PREFIX##_swap_impl(rbuf, PREFIX##_index_impl(rbuf, j), PREFIX##_index_impl(rbuf, high));                                      \
			return j;                                                                                                                     \
		}                                                                                                                                     \
                                                                                                                                                      \
		static void PREFIX##_quicksort_impl(PREFIX##_RingBuffer* rbuf, int (*cmp)(const TYPE* x, const TYPE* y), size_t low, size_t high) {   \
			if (low < high) {                                                                                                             \
				size_t i = PREFIX##_partition_impl(rbuf, cmp, low, high);                                                             \
				if (i > 0) PREFIX##_quicksort_impl(rbuf, cmp, low, i - 1);                                                            \
				PREFIX##_quicksort_impl(rbuf, cmp, i + 1, high);                                                                      \
			}                                                                                                                             \
		}                                                                                                                                     \
                                                                                                                                                      \
		bool PREFIX##_init(PREFIX##_RingBuffer* rbuf) {                                                                                       \
			assert(rbuf != NULL);                                                                                                         \
			size_t cap = (INIT_CAPACITY) == 0 ? CORE_RINGBUFFER_DEFAULT_CAPACITY : (INIT_CAPACITY);                                       \
			if (cap > ((size_t)-1) / sizeof(TYPE)) return false;                                                                          \
			rbuf->items = (TYPE*)malloc(cap * sizeof(TYPE));                                                                              \
			if (rbuf->items == NULL) return false;                                                                                        \
			rbuf->front = 0;                                                                                                              \
			rbuf->back = 0;                                                                                                               \
			rbuf->size = 0;                                                                                                               \
			rbuf->capacity = cap;                                                                                                         \
			return true;                                                                                                                  \
		}                                                                                                                                     \
                                                                                                                                                      \
		void PREFIX##_free(PREFIX##_RingBuffer* rbuf) {                                                                                       \
			assert(rbuf != NULL);                                                                                                         \
			free(rbuf->items);                                                                                                            \
			rbuf->items = NULL;                                                                                                           \
			rbuf->front = 0;                                                                                                              \
			rbuf->back = 0;                                                                                                               \
			rbuf->size = 0;                                                                                                               \
			rbuf->capacity = 0;                                                                                                           \
		}                                                                                                                                     \
                                                                                                                                                      \
		bool PREFIX##_is_full(const PREFIX##_RingBuffer* rbuf) {                                                                              \
			assert(rbuf != NULL);                                                                                                         \
			return rbuf->size >= rbuf->capacity;                                                                                          \
		}                                                                                                                                     \
                                                                                                                                                      \
		bool PREFIX##_is_empty(const PREFIX##_RingBuffer* rbuf) {                                                                             \
			assert(rbuf != NULL);                                                                                                         \
			return rbuf->size == 0;                                                                                                       \
		}                                                                                                                                     \
                                                                                                                                                      \
		void PREFIX##_clear(PREFIX##_RingBuffer* rbuf) {                                                                                      \
			assert(rbuf != NULL);                                                                                                         \
			rbuf->front = 0;                                                                                                              \
			rbuf->back = 0;                                                                                                               \
			rbuf->size = 0;                                                                                                               \
		}                                                                                                                                     \
                                                                                                                                                      \
		bool PREFIX##_push_front(PREFIX##_RingBuffer* rbuf, TYPE item) {                                                                      \
			assert(rbuf != NULL);                                                                                                         \
			if (PREFIX##_is_full(rbuf)) {                                                                                                 \
				if (!PREFIX##_grow_impl(rbuf)) return false;                                                                          \
			}                                                                                                                             \
       			if (PREFIX##_is_empty(rbuf)) {                                                                                                \
       				rbuf->front = 0;                                                                                                      \
				rbuf->back = 0;                                                                                                       \
			}                                                                                                                             \
			else {                                                                                                                        \
       				rbuf->front = PREFIX##_dec_impl(rbuf, rbuf->front);                                                                   \
			}                                                                                                                             \
			rbuf->items[rbuf->front] = item;                                                                                              \
			rbuf->size++;                                                                                                                 \
			return true;                                                                                                                  \
		}                                                                                                                                     \
                                                                                                                                                      \
		bool PREFIX##_push_back(PREFIX##_RingBuffer* rbuf, TYPE item) {                                                                       \
			assert(rbuf != NULL);                                                                                                         \
			if (PREFIX##_is_full(rbuf)) {                                                                                                 \
				if (!PREFIX##_grow_impl(rbuf)) return false;                                                                          \
			}                                                                                                                             \
       			if (PREFIX##_is_empty(rbuf)) {                                                                                                \
       				rbuf->front = 0;                                                                                                      \
				rbuf->back = 0;                                                                                                       \
			}                                                                                                                             \
			else {                                                                                                                        \
       				rbuf->back = PREFIX##_inc_impl(rbuf, rbuf->back);                                                                     \
			}                                                                                                                             \
			rbuf->items[rbuf->back] = item;                                                                                               \
			rbuf->size++;                                                                                                                 \
			return true;                                                                                                                  \
		}                                                                                                                                     \
                                                                                                                                                      \
		bool PREFIX##_pop_front(PREFIX##_RingBuffer* rbuf, TYPE* out) {                                                                       \
			assert(rbuf != NULL);                                                                                                         \
			assert(out != NULL);                                                                                                          \
			if (PREFIX##_is_empty(rbuf)) return false;                                                                                    \
			*out = rbuf->items[rbuf->front];                                                                                              \
			rbuf->size--;                                                                                                                 \
			if (PREFIX##_is_empty(rbuf)) {                                                                                                \
       				rbuf->front = 0;                                                                                                      \
				rbuf->back = 0;                                                                                                       \
			}                                                                                                                             \
       			else {                                                                                                                        \
				rbuf->front = PREFIX##_inc_impl(rbuf, rbuf->front);                                                                   \
			}                                                                                                                             \
			return true;                                                                                                                  \
		}                                                                                                                                     \
                                                                                                                                                      \
		bool PREFIX##_pop_back(PREFIX##_RingBuffer* rbuf, TYPE* out) {                                                                        \
			assert(rbuf != NULL);                                                                                                         \
			assert(out != NULL);                                                                                                          \
			if (PREFIX##_is_empty(rbuf)) return false;                                                                                    \
			*out = rbuf->items[rbuf->back];                                                                                               \
			rbuf->size--;                                                                                                                 \
			if (PREFIX##_is_empty(rbuf)) {                                                                                                \
       				rbuf->front = 0;                                                                                                      \
				rbuf->back = 0;                                                                                                       \
			}                                                                                                                             \
       			else {                                                                                                                        \
				rbuf->back = PREFIX##_dec_impl(rbuf, rbuf->back);                                                                     \
			}                                                                                                                             \
			return true;                                                                                                                  \
		}                                                                                                                                     \
                                                                                                                                                      \
		bool PREFIX##_insert_at(PREFIX##_RingBuffer* rbuf, size_t index, TYPE item) {                                                         \
			assert(rbuf != NULL);                                                                                                         \
			if (index > rbuf->size) return false;                                                                                         \
			if (PREFIX##_is_full(rbuf)) {                                                                                                 \
				if (!PREFIX##_grow_impl(rbuf)) return false;                                                                          \
			}                                                                                                                             \
                                                                                                                                                      \
			size_t left = index;                                                                                                          \
			size_t right = rbuf->size - index;                                                                                            \
			if (left < right) {                                                                                                           \
				size_t prev_front = rbuf->front;                                                                                      \
				size_t new_front = PREFIX##_dec_impl(rbuf, prev_front);                                                               \
				for (size_t i = 0; i < index; i++) {                                                                                  \
	 				rbuf->items[(new_front + i) % rbuf->capacity] = rbuf->items[(prev_front + i) % rbuf->capacity];               \
				}                                                                                                                     \
				rbuf->front = new_front;                                                                                              \
				rbuf->items[PREFIX##_index_impl(rbuf, index)] = item;                                                                 \
			}                                                                                                                             \
			else {                                                                                                                        \
				for (size_t i = rbuf->size; i > index; i--) {                                                                         \
					rbuf->items[PREFIX##_index_impl(rbuf, i)] = rbuf->items[(PREFIX##_index_impl(rbuf, i - 1))];                  \
				}                                                                                                                     \
				rbuf->items[PREFIX##_index_impl(rbuf, index)] = item;                                                                 \
			}                                                                                                                             \
                                                                                                                                                      \
			rbuf->size++;                                                                                                                 \
			rbuf->back = PREFIX##_index_impl(rbuf, rbuf->size - 1);                                                                       \
			return true;                                                                                                                  \
		}                                                                                                                                     \
                                                                                                                                                      \
		bool PREFIX##_erase_at(PREFIX##_RingBuffer* rbuf, size_t index) {                                                                     \
			assert(rbuf != NULL);                                                                                                         \
			if (index >= rbuf->size) return false;                                                                                        \
                                                                                                                                                      \
			size_t left = index;                                                                                                          \
			size_t right = rbuf->size - index - 1;                                                                                        \
			if (left < right) {                                                                                                           \
				size_t prev_front = rbuf->front;                                                                                      \
				for (size_t i = index; i > 0; i--) {                                                                                  \
	 				rbuf->items[(prev_front + i) % rbuf->capacity] = rbuf->items[(prev_front + i - 1) % rbuf->capacity];          \
				}                                                                                                                     \
				rbuf->front = PREFIX##_inc_impl(rbuf, prev_front);                                                                    \
			}                                                                                                                             \
			else {                                                                                                                        \
				for (size_t i = index; i < rbuf->size - 1; i++) {                                                                     \
					rbuf->items[PREFIX##_index_impl(rbuf, i)] = rbuf->items[(PREFIX##_index_impl(rbuf, i + 1))];                  \
				}                                                                                                                     \
			}                                                                                                                             \
                                                                                                                                                      \
			rbuf->size--;                                                                                                                 \
			if (PREFIX##_is_empty(rbuf)) {                                                                                                \
        			rbuf->front = 0;                                                                                                      \
				rbuf->back = 0;                                                                                                       \
			}                                                                                                                             \
        			else {                                                                                                                \
				rbuf->back = PREFIX##_index_impl(rbuf, rbuf->size - 1);                                                               \
			}                                                                                                                             \
			return true;                                                                                                                  \
		}                                                                                                                                     \
                                                                                                                                                      \
		bool PREFIX##_at(const PREFIX##_RingBuffer* rbuf, size_t index, TYPE* out) {                                                          \
			assert(rbuf != NULL);                                                                                                         \
			assert(out != NULL);                                                                                                          \
			if (index >= rbuf->size) return false;                                                                                        \
			index = PREFIX##_index_impl(rbuf, index);                                                                                     \
			*out = rbuf->items[index];                                                                                                    \
			return true;                                                                                                                  \
		}                                                                                                                                     \
                                                                                                                                                      \
		size_t PREFIX##_find_index(const PREFIX##_RingBuffer* rbuf, int (*cmp)(const TYPE* x, const TYPE* y), TYPE item) {                    \
			assert(rbuf != NULL);                                                                                                         \
			assert(cmp != NULL);                                                                                                          \
			for (size_t i = 0; i < rbuf->size; i++) {                                                                                     \
				if (cmp(&item, &rbuf->items[PREFIX##_index_impl(rbuf, i)]) == 0) return i;                                            \
			}                                                                                                                             \
			return rbuf->size;                                                                                                            \
		}                                                                                                                                     \
                                                                                                                                                      \
		bool PREFIX##_find(const PREFIX##_RingBuffer* rbuf, int (*cmp)(const TYPE* x, const TYPE* y), TYPE item, TYPE* out) {                 \
			assert(rbuf != NULL);                                                                                                         \
			assert(out != NULL);                                                                                                          \
			assert(cmp != NULL);                                                                                                          \
			for (size_t i = 0; i < rbuf->size; i++) {                                                                                     \
				if (cmp(&item, &rbuf->items[PREFIX##_index_impl(rbuf, i)]) == 0) {                                                    \
					*out = rbuf->items[PREFIX##_index_impl(rbuf, i)];                                                             \
					return true;                                                                                                  \
				}                                                                                                                     \
			}                                                                                                                             \
			return false;                                                                                                                 \
		}                                                                                                                                     \
                                                                                                                                                      \
		void PREFIX##_sort(PREFIX##_RingBuffer* rbuf, int (*cmp)(const TYPE* x, const TYPE* y)) {                                             \
			assert(rbuf != NULL);                                                                                                         \
			assert(cmp != NULL);                                                                                                          \
			if (rbuf->size == 0) return;                                                                                                  \
			PREFIX##_quicksort_impl(rbuf, cmp, 0, rbuf->size - 1);                                                                        \
		}                                                                                                                                     \
	)
