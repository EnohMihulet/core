#pragma once

#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

/*
	NOTE:
	#define CORE_QUEUE_IMPLEMENTATION
	before you include this file in only one C or C++ file

	NOTE:
	Safe only for plain old data / trivially copyable types.
	Not suitable for C++ types with constructors, destructors, or invariants.

QUEUE_DEFINE(PREFIX, TYPE, INIT_CAPACITY) generates:

typedef struct {
	size_t size, capacity, front, back;
	TYPE* items;
} PREFIX##_Queue;

bool PREFIX##_init(PREFIX##_Queue* q);
void PREFIX##_free(PREFIX##_Queue* q);
bool PREFIX##_is_full(const PREFIX##_Queue* q);
bool PREFIX##_is_empty(const PREFIX##_Queue* q);
void PREFIX##_clear(PREFIX##_Queue* q);
bool PREFIX##_enqueue(PREFIX##_Queue* q, TYPE item);
bool PREFIX##_dequeue(PREFIX##_Queue* q, TYPE* out);
bool PREFIX##_front(PREFIX##_Queue* q, TYPE* out);
*/

#define CORE_QUEUE_GROWTH_RATE 2
#define CORE_QUEUE_DEFAULT_CAPACITY 8

#ifdef CORE_QUEUE_IMPLEMENTATION
	#define CORE_QUEUE_IMPL(...) __VA_ARGS__
#else
	#define CORE_QUEUE_IMPL(...)
#endif

#define QUEUE_DEFINE(PREFIX, TYPE, INIT_CAPACITY)                                                          \
                                                                                                           \
	typedef struct {                                                                                   \
		size_t size, capacity, front, back;                                                        \
		TYPE* items;                                                                               \
	} PREFIX##_Queue;                                                                                  \
                                                                                                           \
	bool PREFIX##_init(PREFIX##_Queue* q);                                                             \
	void PREFIX##_free(PREFIX##_Queue* q);                                                             \
	bool PREFIX##_is_full(const PREFIX##_Queue* q);                                                    \
	bool PREFIX##_is_empty(const PREFIX##_Queue* q);                                                   \
	void PREFIX##_clear(PREFIX##_Queue* q);                                                            \
	bool PREFIX##_enqueue(PREFIX##_Queue* q, TYPE item);                                               \
	bool PREFIX##_dequeue(PREFIX##_Queue* q, TYPE* out);                                               \
	bool PREFIX##_front(PREFIX##_Queue* q, TYPE* out);                                                 \
                                                                                                           \
	CORE_QUEUE_IMPL(                                                                                   \
		static size_t PREFIX##_index_impl(const PREFIX##_Queue* q, size_t i) {                     \
			return (q->front + i) % q->capacity;                                               \
		}                                                                                          \
                                                                                                           \
		static size_t PREFIX##_inc_impl(const PREFIX##_Queue* q, size_t i) {                       \
			return (i + 1 == q->capacity) ? 0 : i + 1;                                         \
		}                                                                                          \
                                                                                                           \
		static size_t PREFIX##_dec_impl(const PREFIX##_Queue* q, size_t i) {                       \
			return (i == 0) ? q->capacity - 1 : i - 1;                                         \
		}                                                                                          \
		static bool PREFIX##_grow_impl(PREFIX##_Queue* q) {                                        \
			assert(q != NULL);                                                                 \
			size_t new_capacity;                                                               \
			if (q->capacity == 0) {                                                            \
				new_capacity = CORE_QUEUE_DEFAULT_CAPACITY;                                \
			} else {                                                                           \
				if (CORE_QUEUE_GROWTH_RATE <= 1) return false;                             \
				if (q->capacity > ((size_t)-1) / CORE_QUEUE_GROWTH_RATE) return false;     \
				new_capacity = q->capacity * CORE_QUEUE_GROWTH_RATE;                       \
				if (new_capacity <= q->capacity) return false;                             \
			}                                                                                  \
			if (new_capacity > ((size_t)-1) / sizeof(TYPE)) return false;                      \
			TYPE* new_items = (TYPE*)malloc(new_capacity * sizeof(TYPE));                      \
			if (new_items == NULL) return false;                                               \
			for (size_t i = 0; i < q->size; i++) {                                             \
				new_items[i] = q->items[PREFIX##_index_impl(q, i)];                        \
			}                                                                                  \
			free(q->items);                                                                    \
			q->items = new_items;                                                              \
			q->front = 0;                                                                      \
			q->back = q->size ? q->size - 1 : 0;                                               \
			q->capacity = new_capacity;                                                        \
			return true;                                                                       \
		}                                                                                          \
                                                                                                           \
		bool PREFIX##_init(PREFIX##_Queue* q) {                                                    \
			assert(q != NULL);                                                                 \
			size_t cap = (INIT_CAPACITY) == 0 ? CORE_QUEUE_DEFAULT_CAPACITY : (INIT_CAPACITY); \
			if (cap > ((size_t)-1) / sizeof(TYPE)) return false;                               \
			q->items = (TYPE*)malloc(cap * sizeof(TYPE));                                      \
			if (q->items == NULL) return false;                                                \
			q->front = 0;                                                                      \
			q->back = 0;                                                                       \
			q->size = 0;                                                                       \
			q->capacity = cap;                                                                 \
			return true;                                                                       \
		}                                                                                          \
                                                                                                           \
		void PREFIX##_free(PREFIX##_Queue* q) {                                                    \
			assert(q != NULL);                                                                 \
			free(q->items);                                                                    \
			q->items = NULL;                                                                   \
			q->front = 0;                                                                      \
			q->back = 0;                                                                       \
			q->size = 0;                                                                       \
			q->capacity = 0;                                                                   \
		}                                                                                          \
                                                                                                           \
		bool PREFIX##_is_full(const PREFIX##_Queue* q) {                                           \
			assert(q != NULL);                                                                 \
			return q->size >= q->capacity;                                                     \
		}                                                                                          \
                                                                                                           \
		bool PREFIX##_is_empty(const PREFIX##_Queue* q) {                                          \
			assert(q != NULL);                                                                 \
			return q->size == 0;                                                               \
		}                                                                                          \
                                                                                                           \
		void PREFIX##_clear(PREFIX##_Queue* q) {                                                   \
			assert(q != NULL);                                                                 \
			q->front = 0;                                                                      \
			q->back = 0;                                                                       \
			q->size = 0;                                                                       \
		}                                                                                          \
                                                                                                           \
		bool PREFIX##_enqueue(PREFIX##_Queue* q, TYPE item) {                                      \
			assert(q != NULL);                                                                 \
			if (PREFIX##_is_full(q)) {                                                         \
				if (!PREFIX##_grow_impl(q)) return false;                                  \
			}                                                                                  \
       			if (PREFIX##_is_empty(q)) {                                                        \
       				q->front = 0;                                                              \
				q->back = 0;                                                               \
			}                                                                                  \
			else {                                                                             \
       				q->front = PREFIX##_dec_impl(q, q->front);                                 \
			}                                                                                  \
			q->items[q->front] = item;                                                         \
			q->size++;                                                                         \
			return true;                                                                       \
		}                                                                                          \
                                                                                                           \
		bool PREFIX##_dequeue(PREFIX##_Queue* q, TYPE* out) {                                      \
			assert(q != NULL);                                                                 \
			assert(out != NULL);                                                               \
			if (PREFIX##_is_empty(q)) return false;                                            \
			*out = q->items[q->back];                                                          \
			q->size--;                                                                         \
			if (PREFIX##_is_empty(q)) {                                                        \
       				q->front = 0;                                                              \
				q->back = 0;                                                               \
			}                                                                                  \
       			else {                                                                             \
				q->back = PREFIX##_dec_impl(q, q->back);                                   \
			}                                                                                  \
			return true;                                                                       \
		}                                                                                          \
                                                                                                           \
		bool PREFIX##_front(PREFIX##_Queue* q, TYPE* out) {                                        \
			assert(q != NULL);                                                                 \
			assert(out != NULL);                                                               \
			if (PREFIX##_is_empty(q)) return false;                                            \
       			*out = q->items[q->back];                                                          \
			return true;                                                                       \
		}                                                                                          \
	)
