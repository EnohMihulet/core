#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "test.h"

#define CORE_ARENA_IMPLEMENTATION
#include "../Memory/arena.h"

ARENA_DEFINE(test_arena)

static int tests_run = 0;
static int tests_failed = 0;

static bool test_owned_lifecycle(void) {
	test_arena_Arena arena;

	CHECK(test_arena_init(&arena, 128));
	CHECK(arena.buffer != NULL);
	CHECK(arena.size == 0);
	CHECK(arena.capacity == 128);
	CHECK(arena.owns_buffer);
	CHECK(test_arena_remaining(&arena) == 128);

	void* first = test_arena_alloc(&arena, 12);
	CHECK(first != NULL);
	CHECK(test_arena_contains(&arena, first));
	CHECK(test_arena_remaining(&arena) <= 116);

	test_arena_reset(&arena);
	CHECK(arena.size == 0);
	CHECK(test_arena_remaining(&arena) == 128);
	CHECK(test_arena_alloc(&arena, 12) == first);

	test_arena_free(&arena);
	CHECK(arena.buffer == NULL);
	CHECK(arena.size == 0);
	CHECK(arena.capacity == 0);
	CHECK(!arena.owns_buffer);
	return true;
}

static bool test_alignment_and_capacity(void) {
	test_arena_Arena arena;

	CHECK(test_arena_init(&arena, 128));
	CHECK(test_arena_alloc(&arena, 3) != NULL);
	void* aligned = test_arena_alloc_aligned(&arena, 16, 32);
	CHECK(aligned != NULL);
	CHECK((uintptr_t)aligned % 32 == 0);
	CHECK(test_arena_contains(&arena, aligned));
	CHECK(test_arena_alloc_aligned(&arena, 128, 16) == NULL);
	CHECK(test_arena_alloc_aligned(&arena, 1, 3) == NULL);

	test_arena_free(&arena);
	return true;
}

static bool test_calloc_and_strdup(void) {
	test_arena_Arena arena;

	CHECK(test_arena_init(&arena, 256));
	unsigned int* values = test_arena_calloc(&arena, 8, sizeof(*values));
	CHECK(values != NULL);
	for (size_t i = 0; i < 8; ++i) {
		CHECK(values[i] == 0);
		values[i] = (unsigned int)i;
	}

	char* copy = test_arena_strdup(&arena, "arena string");
	CHECK(copy != NULL);
	CHECK(strcmp(copy, "arena string") == 0);
	CHECK(test_arena_contains(&arena, copy));
	CHECK(test_arena_calloc(&arena, SIZE_MAX, 2) == NULL);

	test_arena_free(&arena);
	return true;
}

static bool test_external_buffer(void) {
	max_align_t storage[16];
	test_arena_Arena arena;

	CHECK(test_arena_init_with_buffer(&arena, storage, sizeof(storage)));
	CHECK(arena.buffer == (unsigned char*)storage);
	CHECK(arena.capacity == sizeof(storage));
	CHECK(!arena.owns_buffer);

	void* allocation = test_arena_alloc(&arena, sizeof(storage) / 2);
	CHECK(allocation != NULL);
	CHECK(test_arena_contains(&arena, allocation));
	CHECK(!test_arena_contains(&arena, &arena));

	test_arena_free(&arena);
	CHECK(arena.buffer == NULL);
	((unsigned char*)storage)[0] = 0x5a;
	CHECK(((unsigned char*)storage)[0] == 0x5a);
	return true;
}

int main(void) {
	RUN_TEST(test_owned_lifecycle);
	RUN_TEST(test_alignment_and_capacity);
	RUN_TEST(test_calloc_and_strdup);
	RUN_TEST(test_external_buffer);

	if (tests_failed != 0) {
		fprintf(stderr, "\n%d/%d tests failed\n", tests_failed, tests_run);
		return 1;
	}
	fprintf(stdout, "\nAll %d tests passed\n", tests_run);
	return 0;
}
