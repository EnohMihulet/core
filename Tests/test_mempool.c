#include <stdbool.h>
#include <stdio.h>

#include "test.h"

typedef struct TestItem {
	struct TestItem* next;
	int id;
	double value;
} TestItem;

#define CORE_MEMPOOL_IMPLEMENTATION
#include "../Memory/mempool.h"

MEMPOOL_DEFINE(test_pool, TestItem, 4)

static int tests_run = 0;
static int tests_failed = 0;

static bool test_owned_lifecycle(void) {
	test_pool_MemPool pool;

	CHECK(test_pool_init(&pool));
	CHECK(pool.buffer != NULL);
	CHECK(pool.size == 0);
	CHECK(pool.capacity == 4);
	CHECK(pool.owns_buffer);
	CHECK(test_pool_is_empty(&pool));
	CHECK(!test_pool_is_full(&pool));
	CHECK(test_pool_available(&pool) == 4);

	test_pool_free(&pool);
	CHECK(pool.buffer == NULL);
	CHECK(pool.free_list == NULL);
	CHECK(pool.size == 0);
	CHECK(pool.capacity == 0);
	CHECK(!pool.owns_buffer);
	return true;
}

static bool test_allocate_release_and_reuse(void) {
	test_pool_MemPool pool;
	TestItem* items[4];

	CHECK(test_pool_init(&pool));
	for (size_t i = 0; i < 4; ++i) {
		items[i] = test_pool_alloc(&pool);
		CHECK(items[i] != NULL);
		CHECK(test_pool_contains(&pool, items[i]));
		for (size_t j = 0; j < i; ++j) {
			CHECK(items[i] != items[j]);
		}
		items[i]->id = (int)i;
	}
	CHECK(test_pool_is_full(&pool));
	CHECK(test_pool_available(&pool) == 0);
	CHECK(test_pool_alloc(&pool) == NULL);

	CHECK(test_pool_release(&pool, items[1]));
	CHECK(!test_pool_is_full(&pool));
	CHECK(test_pool_available(&pool) == 1);
	CHECK(!test_pool_release(&pool, items[1]));

	TestItem* reused = test_pool_alloc(&pool);
	CHECK(reused == items[1]);
	CHECK(test_pool_is_full(&pool));

	test_pool_free(&pool);
	return true;
}

static bool test_rejects_foreign_and_misaligned_items(void) {
	test_pool_MemPool pool;
	TestItem outsider = {0};

	CHECK(test_pool_init(&pool));
	TestItem* item = test_pool_alloc(&pool);
	CHECK(item != NULL);
	CHECK(!test_pool_contains(&pool, &outsider));
	CHECK(!test_pool_release(&pool, &outsider));
	CHECK(!test_pool_release(&pool, (TestItem*)((unsigned char*)item + 1)));
	CHECK(pool.size == 1);

	test_pool_free(&pool);
	return true;
}

static bool test_clear_and_external_buffer(void) {
	TestItem storage[3];
	test_pool_MemPool pool;

	CHECK(test_pool_init_with_buffer(&pool, storage, 3));
	CHECK(pool.buffer == storage);
	CHECK(pool.capacity == 3);
	CHECK(!pool.owns_buffer);
	CHECK(test_pool_alloc(&pool) != NULL);
	CHECK(test_pool_alloc(&pool) != NULL);

	test_pool_clear(&pool);
	CHECK(test_pool_is_empty(&pool));
	CHECK(test_pool_available(&pool) == 3);
	CHECK(pool.capacity == 3);
	for (size_t i = 0; i < 3; ++i) {
		CHECK(test_pool_alloc(&pool) != NULL);
	}
	CHECK(test_pool_is_full(&pool));

	test_pool_free(&pool);
	CHECK(pool.buffer == NULL);
	storage[0].id = 7;
	CHECK(storage[0].id == 7);
	return true;
}

int main(void) {
	RUN_TEST(test_owned_lifecycle);
	RUN_TEST(test_allocate_release_and_reuse);
	RUN_TEST(test_rejects_foreign_and_misaligned_items);
	RUN_TEST(test_clear_and_external_buffer);

	if (tests_failed != 0) {
		fprintf(stderr, "\n%d/%d tests failed\n", tests_failed, tests_run);
		return 1;
	}
	fprintf(stdout, "\nAll %d tests passed\n", tests_run);
	return 0;
}
