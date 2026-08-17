#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "test.h"

#define CORE_HASHMAP_IMPLEMENTATION
#include "../DataStructures/hashmap.h"

static uint64_t hash_int(const int* key) {
	return (uint64_t)(*key % 4);
}

static bool equal_int(const int* x, const int* y) {
	return *x == *y;
}

HASHMAP_DEFINE(test_map, int, int, 4)

static int tests_run = 0;
static int tests_failed = 0;

static bool test_init_clear_and_free(void) {
	test_map_HashMap map;

	CHECK(test_map_init(&map, hash_int, equal_int));
	CHECK(map.size == 0);
	CHECK(map.capacity >= 4);
	CHECK(map.entries != NULL);
	CHECK(map.hash == hash_int);
	CHECK(map.equals == equal_int);
	CHECK(test_map_is_empty(&map));
	CHECK(test_map_load_factor(&map) == 0.0f);

	CHECK(test_map_insert(&map, 1, 10));
	test_map_clear(&map);
	CHECK(test_map_is_empty(&map));
	CHECK(map.size == 0);
	CHECK(map.capacity >= 4);
	CHECK(!test_map_contains(&map, 1));

	test_map_free(&map);
	CHECK(map.size == 0);
	CHECK(map.capacity == 0);
	CHECK(map.entries == NULL);
	return true;
}

static bool test_insert_get_and_set(void) {
	test_map_HashMap map;
	int value = -1;

	CHECK(test_map_init(&map, hash_int, equal_int));
	CHECK(test_map_insert(&map, 1, 10));
	CHECK(test_map_insert(&map, 5, 50));
	CHECK(test_map_insert(&map, 9, 90));
	CHECK(map.size == 3);
	CHECK(test_map_contains(&map, 1));
	CHECK(test_map_contains(&map, 5));
	CHECK(test_map_get(&map, 9, &value) && value == 90);

	CHECK(!test_map_insert(&map, 5, 500));
	CHECK(test_map_get(&map, 5, &value) && value == 50);
	CHECK(test_map_set(&map, 5, 500));
	CHECK(test_map_get(&map, 5, &value) && value == 500);
	CHECK(map.size == 3);

	CHECK(test_map_set(&map, 13, 130));
	CHECK(test_map_get(&map, 13, &value) && value == 130);
	CHECK(map.size == 4);

	int* value_ptr = test_map_get_ptr(&map, 9);
	CHECK(value_ptr != NULL);
	*value_ptr = 99;
	CHECK(test_map_get(&map, 9, &value) && value == 99);
	CHECK(test_map_get_ptr(&map, 1000) == NULL);

	value = -1;
	CHECK(!test_map_get(&map, 1000, &value));
	CHECK(value == -1);
	test_map_free(&map);
	return true;
}

static bool test_remove_and_tombstone_reuse(void) {
	test_map_HashMap map;
	int value = 0;

	CHECK(test_map_init(&map, hash_int, equal_int));
	CHECK(test_map_insert(&map, 1, 10));
	CHECK(test_map_insert(&map, 5, 50));
	CHECK(test_map_insert(&map, 9, 90));
	CHECK(test_map_remove(&map, 5));
	CHECK(!test_map_contains(&map, 5));
	CHECK(!test_map_remove(&map, 5));
	CHECK(map.size == 2);

	CHECK(test_map_get(&map, 1, &value) && value == 10);
	CHECK(test_map_get(&map, 9, &value) && value == 90);
	CHECK(test_map_insert(&map, 13, 130));
	CHECK(test_map_get(&map, 13, &value) && value == 130);

	test_map_free(&map);
	return true;
}

static bool test_growth_and_reserve(void) {
	test_map_HashMap map;

	CHECK(test_map_init(&map, hash_int, equal_int));
	CHECK(test_map_reserve(&map, 64));
	CHECK(map.capacity >= 64);

	for (int i = 0; i < 100; ++i) {
		CHECK(test_map_set(&map, i, i * 10));
	}
	CHECK(map.size == 100);
	CHECK(test_map_load_factor(&map) > 0.0f);
	CHECK(test_map_load_factor(&map) <= 1.0f);

	for (int i = 0; i < 100; ++i) {
		int value = -1;
		CHECK(test_map_get(&map, i, &value));
		CHECK(value == i * 10);
	}

	test_map_free(&map);
	return true;
}

int main(void) {
	RUN_TEST(test_init_clear_and_free);
	RUN_TEST(test_insert_get_and_set);
	RUN_TEST(test_remove_and_tombstone_reuse);
	RUN_TEST(test_growth_and_reserve);

	if (tests_failed != 0) {
		fprintf(stderr, "\n%d/%d tests failed\n", tests_failed, tests_run);
		return 1;
	}
	fprintf(stdout, "\nAll %d tests passed\n", tests_run);
	return 0;
}
