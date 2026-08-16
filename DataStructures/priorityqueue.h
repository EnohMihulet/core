#pragma once

#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

/*
	NOTE:
	#define CORE_PRIORITYQUEUE_IMPLEMENTATION
	before you include this file in only one C or C++ file

	NOTE:
	Safe only for plain old data / trivially copyable types.
	Not suitable for C++ types with constructors, destructors, or invariants.

PRIORITYQUEUE_DEFINE(PREFIX, TYPE, INIT_CAPACITY) generates:

	typedef struct PREFIX##_PrioQueue {
		size_t size, capacity;
		int (*cmp)(const TYPE* x, const TYPE* y);
		TYPE* items;
	} PREFIX##_PrioQueue;

	bool PREFIX##_init(PREFIX##_PrioQueue* pq, int (*cmp)(const TYPE* x, const TYPE* y));
	void PREFIX##_free(PREFIX##_PrioQueue* pq);
	bool PREFIX##_is_empty(PREFIX##_PrioQueue* pq);
	bool PREFIX##_is_full(PREFIX##_PrioQueue* pq);
	void PREFIX##_clear(PREFIX##_PrioQueue* pq);
	bool PREFIX##_reserve(PREFIX##_PrioQueue* pq, size_t capacity);
	bool PREFIX##_insert(PREFIX##_PrioQueue* pq, TYPE item);
	bool PREFIX##_extract(PREFIX##_PrioQueue* pq, TYPE* out);
	bool PREFIX##_peek(PREFIX##_PrioQueue* pq, TYPE* out);

cmp contract:
- cmp(&x, &y) < 0  => x comes before y
- cmp(&x, &y) == 0 => x and y compare equal
- cmp(&x, &y) > 0  => x comes after y
*/

#define CORE_PRIORITYQUEUE_GROWTH_RATE 2
#define CORE_PRIORITYQUEUE_DEFAULT_CAPACITY 8

#ifdef CORE_PRIORITYQUEUE_IMPLEMENTATION
	#define CORE_PRIORITYQUEUE_IMPL(...) __VA_ARGS__
#else
	#define CORE_PRIORITYQUEUE_IMPL(...)
#endif

#define PRIORITYQUEUE_DEFINE(PREFIX, TYPE, INIT_CAPACITY)                                                  \
                                                                                                           \
	typedef struct PREFIX##_PrioQueue {                                                                \
		size_t size, capacity;                                                                     \
		int (*cmp)(const TYPE* x, const TYPE* y);                                                  \
		TYPE* items;                                                                               \
	} PREFIX##_PrioQueue;                                                                              \
                                                                                                           \
	bool PREFIX##_init(PREFIX##_PrioQueue* pq, int (*cmp)(const TYPE* x, const TYPE* y));              \
	void PREFIX##_free(PREFIX##_PrioQueue* pq);                                                        \
	bool PREFIX##_is_empty(PREFIX##_PrioQueue* pq);                                                    \
	bool PREFIX##_is_full(PREFIX##_PrioQueue* pq);                                                     \
	void PREFIX##_clear(PREFIX##_PrioQueue* pq);                                                       \
	bool PREFIX##_reserve(PREFIX##_PrioQueue* pq, size_t capacity);                                    \
	bool PREFIX##_insert(PREFIX##_PrioQueue* pq, TYPE item);                                           \
	bool PREFIX##_extract(PREFIX##_PrioQueue* pq, TYPE* out);                                          \
	bool PREFIX##_peek(PREFIX##_PrioQueue* pq, TYPE* out);                                             \
                                                                                                            \
	CORE_PRIORITYQUEUE_IMPL(                                                                           \
                                                                                                           \
	size_t PREFIX##_parent_impl(size_t i) {                                                            \
		return (i - 1) / 2;                                                                        \
	}                                                                                                  \
                                                                                                           \
	size_t PREFIX##_left_impl(size_t i) {                                                              \
		return i * 2 + 1;                                                                          \
	}                                                                                                  \
                                                                                                           \
	size_t PREFIX##_right_impl(size_t i) {                                                             \
		return i * 2 + 2;                                                                          \
	}                                                                                                  \
	static bool PREFIX##_reserve_impl(PREFIX##_PrioQueue* pq, size_t min_capacity) {                   \
		assert(pq != NULL);                                                                        \
		if (min_capacity <= pq->capacity) return true;                                             \
		if (min_capacity > ((size_t)-1) / sizeof(TYPE)) return false;                              \
		TYPE* new_items = (TYPE*)realloc(pq->items, min_capacity * sizeof(TYPE));                  \
		if (new_items == NULL) return false;                                                       \
		pq->items = new_items;                                                                     \
		pq->capacity = min_capacity;                                                               \
		return true;                                                                               \
	}                                                                                                  \
                                                                                                           \
	static bool PREFIX##_grow_impl(PREFIX##_PrioQueue* pq) {                                           \
		assert(pq != NULL);                                                                        \
                                                                                                           \
		size_t new_capacity;                                                                       \
		if (pq->capacity == 0) {                                                                   \
			new_capacity = CORE_PRIORITYQUEUE_DEFAULT_CAPACITY;                                \
		} else {                                                                                   \
			if (CORE_PRIORITYQUEUE_GROWTH_RATE <= 1) return false;                             \
			if (pq->capacity > ((size_t)-1) / CORE_PRIORITYQUEUE_GROWTH_RATE) return false;    \
			new_capacity = pq->capacity * CORE_PRIORITYQUEUE_GROWTH_RATE;                      \
			if (new_capacity <= pq->capacity) return false;                                    \
		}                                                                                          \
		return PREFIX##_reserve_impl(pq, new_capacity);                                            \
	}                                                                                                  \
                                                                                                           \
	static void PREFIX##_swap_impl(PREFIX##_PrioQueue* pq, size_t i, size_t j) {                       \
		assert(pq != NULL);                                                                        \
		TYPE temp = pq->items[i];                                                                  \
		pq->items[i] = pq->items[j];                                                               \
		pq->items[j] = temp;                                                                       \
	}                                                                                                  \
                                                                                                            \
	void PREFIX##_sift_up_impl(PREFIX##_PrioQueue* pq, size_t i) {                                      \
		assert(pq != NULL);                                                                        \
		assert(pq->cmp != NULL);                                                                   \
		while (i > 0) {                                                                            \
			size_t parent = PREFIX##_parent_impl(i);                                             \
			if (pq->cmp(&pq->items[i], &pq->items[parent]) >= 0) break;                         \
			PREFIX##_swap_impl(pq, i, parent);                                                   \
			i = parent;                                                                           \
		}                                                                                          \
	}                                                                                                  \
                                                                                                            \
	void PREFIX##_heapify_impl(PREFIX##_PrioQueue* pq, size_t i) {                                     \
		assert(pq != NULL);                                                                        \
		assert(pq->cmp != NULL);                                                                   \
		if (pq->size == 0) return;                                                                 \
		size_t l = PREFIX##_left_impl(i);                                                          \
		size_t r = PREFIX##_right_impl(i);                                                         \
		size_t j = i;                                                                              \
		if (l < pq->size && pq->cmp(&pq->items[l], &pq->items[j]) < 0) j = l;                      \
		if (r < pq->size && pq->cmp(&pq->items[r], &pq->items[j]) < 0) j = r;                      \
		if (i != j) {                                                                              \
			PREFIX##_swap_impl(pq, i, j);                                                      \
			PREFIX##_heapify_impl(pq, j);                                                      \
		}                                                                                          \
	}                                                                                                  \
                                                                                                           \
	bool PREFIX##_init(PREFIX##_PrioQueue* pq, int (*cmp)(const TYPE* x, const TYPE* y)) {             \
		assert(pq != NULL);                                                                        \
		assert(cmp != NULL);                                                                       \
		size_t cap = (INIT_CAPACITY) == 0 ? CORE_PRIORITYQUEUE_DEFAULT_CAPACITY : (INIT_CAPACITY); \
		if (cap > ((size_t)-1) / sizeof(TYPE)) return false;                                       \
		pq->items = (TYPE*)malloc(cap * sizeof(TYPE));                                             \
		if (pq->items == NULL) return false;                                                       \
		pq->size = 0;                                                                              \
		pq->capacity = cap;                                                                        \
		pq->cmp = cmp;                                                                             \
		return true;                                                                               \
	}                                                                                                  \
                                                                                                           \
	void PREFIX##_free(PREFIX##_PrioQueue* pq) {                                                       \
		assert(pq != NULL);                                                                        \
		free(pq->items);                                                                           \
		pq->items = NULL;                                                                          \
		pq->size = 0;                                                                              \
		pq->capacity = 0;                                                                          \
		pq->cmp = NULL;                                                                            \
	}                                                                                                  \
                                                                                                           \
	bool PREFIX##_is_empty(PREFIX##_PrioQueue* pq) {                                                   \
		assert(pq != NULL);                                                                        \
		return pq->size == 0;                                                                      \
	}                                                                                                  \
                                                                                                           \
	bool PREFIX##_is_full(PREFIX##_PrioQueue* pq) {                                                    \
		assert(pq != NULL);                                                                        \
		return pq->size == pq->capacity;                                                           \
	}                                                                                                  \
                                                                                                           \
	void PREFIX##_clear(PREFIX##_PrioQueue* pq) {                                                      \
		assert(pq != NULL);                                                                        \
		pq->size = 0;                                                                              \
	}                                                                                                  \
                                                                                                           \
	bool PREFIX##_reserve(PREFIX##_PrioQueue* pq, size_t capacity) {                                   \
		return PREFIX##_reserve_impl(pq, capacity);                                                \
	}                                                                                                  \
                                                                                                           \
	bool PREFIX##_insert(PREFIX##_PrioQueue* pq, TYPE item) {                                          \
		assert(pq != NULL);                                                                        \
		assert(pq->cmp != NULL);                                                                   \
		if (PREFIX##_is_full(pq)) {                                                                \
			if (!PREFIX##_grow_impl(pq)) return false;                                         \
		}                                                                                          \
		pq->items[pq->size++] = item;                                                              \
		PREFIX##_sift_up_impl(pq, pq->size - 1);                                                   \
		return true;                                                                               \
	}                                                                                                  \
                                                                                                            \
	bool PREFIX##_extract(PREFIX##_PrioQueue* pq, TYPE* out) {                                         \
		assert(pq != NULL);                                                                        \
		assert(out != NULL);                                                                       \
		assert(pq->cmp != NULL);                                                                   \
		if (PREFIX##_is_empty(pq)) return false;                                                   \
	      		PREFIX##_swap_impl(pq, 0, pq->size - 1);                                                   \
		*out = pq->items[--pq->size];                                                              \
		if (!PREFIX##_is_empty(pq)) PREFIX##_heapify_impl(pq, 0);                                  \
		return true;                                                                               \
	}                                                                                                  \
                                                                                                            \
	bool PREFIX##_peek(PREFIX##_PrioQueue* pq, TYPE* out) {                                            \
		assert(pq != NULL);                                                                        \
		assert(out != NULL);                                                                       \
		if (PREFIX##_is_empty(pq)) return false;                                                   \
		*out = pq->items[0];                                                                       \
		return true;                                                                               \
	}                                                                                                  \
                                                                                                           \
		    )
