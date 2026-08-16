#include <stdbool.h>
#include <stdio.h>

#include "test.h"

#define CORE_DYNARRAY_IMPLEMENTATION
#include "../dynarray.h"

static int tests_run = 0;
static int tests_failed = 0;

static int cmp_int_lt(const int* x, const int* y) {
	return (*x > *y) - (*x < *y);
}

static int cmp_int_gt(const int* x, const int* y) {
	return (*x < *y) - (*x > *y);
}

typedef struct TestStruct {
	int x;
	int y;
} TestStruct;

static int cmp_TestStruct_lt(const TestStruct* x, const TestStruct* y) {
	return (x->y > y->y) - (x->y < y->y);
}

static int cmp_TestStruct_gt(const TestStruct* x, const TestStruct* y) {
	return (x->y < y->y) - (x->y > y->y);
}

DYNARRAY_DEFINE(int, int, 4)
DYNARRAY_DEFINE(TestStruct, TestStruct, 4)

static bool test_init(void) {
	int_DynArray iarr;
	CHECK(int_init(&iarr));
	CHECK(iarr.items != NULL);
	CHECK(iarr.size == 0);
	CHECK(iarr.capacity == 4);
	int_free(&iarr);
	CHECK(iarr.items == NULL);
	CHECK(iarr.size == 0);
	CHECK(iarr.capacity == 0);

	TestStruct_DynArray sarr;
	CHECK(TestStruct_init(&sarr));
	CHECK(sarr.items != NULL);
	CHECK(sarr.size == 0);
	CHECK(sarr.capacity == 4);
	TestStruct_free(&sarr);
	CHECK(sarr.items == NULL);
	CHECK(sarr.size == 0);
	CHECK(sarr.capacity == 0);

	return true;
}

static bool test_is_full(void) {
	int_DynArray iarr;
	CHECK(int_init(&iarr));
	CHECK(!int_is_full(&iarr));

	size_t cap = iarr.capacity;
	for (size_t i = 0; i < cap; ++i) {
		CHECK(int_push(&iarr, (int)i));
	}
	CHECK(int_is_full(&iarr));

	int out = 0;
	CHECK(int_pop(&iarr, &out));
	CHECK(!int_is_full(&iarr));
	int_free(&iarr);

	TestStruct_DynArray sarr;
	CHECK(TestStruct_init(&sarr));
	CHECK(!TestStruct_is_full(&sarr));

	cap = sarr.capacity;
	for (size_t i = 0; i < cap; ++i) {
		CHECK(TestStruct_push(&sarr, (TestStruct){.x = (int)i, .y = (int)i}));
	}
	CHECK(TestStruct_is_full(&sarr));

	TestStruct sout = {0};
	CHECK(TestStruct_pop(&sarr, &sout));
	CHECK(!TestStruct_is_full(&sarr));
	TestStruct_free(&sarr);

	return true;
}

static bool test_is_empty(void) {
	int_DynArray iarr;
	CHECK(int_init(&iarr));
	CHECK(int_is_empty(&iarr));
	CHECK(int_push(&iarr, 123));
	CHECK(!int_is_empty(&iarr));
	int_clear(&iarr);
	CHECK(int_is_empty(&iarr));
	int out = 0;
	CHECK(!int_pop(&iarr, &out));
	int_free(&iarr);

	TestStruct_DynArray sarr;
	CHECK(TestStruct_init(&sarr));
	CHECK(TestStruct_is_empty(&sarr));
	CHECK(TestStruct_push(&sarr, (TestStruct){.x = 1, .y = 2}));
	CHECK(!TestStruct_is_empty(&sarr));
	TestStruct_clear(&sarr);
	CHECK(TestStruct_is_empty(&sarr));
	TestStruct sout = {0};
	CHECK(!TestStruct_pop(&sarr, &sout));
	TestStruct_free(&sarr);

	return true;
}

static bool test_reserve(void) {
	int_DynArray iarr;
	int out = 0;

	CHECK(int_init(&iarr));
	CHECK(int_push(&iarr, 11));
	CHECK(int_push(&iarr, 22));
	CHECK(int_push(&iarr, 33));

	const size_t before_cap = iarr.capacity;
	CHECK(int_reserve(&iarr, before_cap + 13));
	CHECK(iarr.capacity >= before_cap + 13);
	CHECK(iarr.size == 3);

	CHECK(int_at(&iarr, 0, &out) && out == 11);
	CHECK(int_at(&iarr, 1, &out) && out == 22);
	CHECK(int_at(&iarr, 2, &out) && out == 33);

	const size_t grown_cap = iarr.capacity;
	CHECK(int_reserve(&iarr, grown_cap));
	CHECK(int_reserve(&iarr, 1));
	CHECK(iarr.capacity == grown_cap);

	int_free(&iarr);

	TestStruct_DynArray sarr;
	TestStruct sout = {0};

	CHECK(TestStruct_init(&sarr));
	CHECK(TestStruct_push(&sarr, (TestStruct){.x = 10, .y = 100}));
	CHECK(TestStruct_push(&sarr, (TestStruct){.x = 20, .y = 200}));

	const size_t before_struct_cap = sarr.capacity;
	CHECK(TestStruct_reserve(&sarr, before_struct_cap + 9));
	CHECK(sarr.capacity >= before_struct_cap + 9);
	CHECK(sarr.size == 2);

	CHECK(TestStruct_at(&sarr, 0, &sout));
	CHECK(sout.x == 10 && sout.y == 100);
	CHECK(TestStruct_at(&sarr, 1, &sout));
	CHECK(sout.x == 20 && sout.y == 200);

	const size_t grown_struct_cap = sarr.capacity;
	CHECK(TestStruct_reserve(&sarr, grown_struct_cap));
	CHECK(TestStruct_reserve(&sarr, 1));
	CHECK(sarr.capacity == grown_struct_cap);

	TestStruct_free(&sarr);

	return true;
}

static bool test_push(void) {
	int_DynArray iarr;
	CHECK(int_init(&iarr));

	for (int i = 0; i < 32; ++i) {
		CHECK(int_push(&iarr, i));
	}
	CHECK(iarr.size == 32);

	for (size_t i = 0; i < iarr.size; ++i) {
		int out = -1;
		CHECK(int_at(&iarr, i, &out));
		CHECK(out == (int)i);
	}
	int_free(&iarr);

	TestStruct_DynArray sarr;
	CHECK(TestStruct_init(&sarr));

	for (int i = 0; i < 32; ++i) {
		CHECK(TestStruct_push(&sarr, (TestStruct){.x = i, .y = i * 10}));
	}
	CHECK(sarr.size == 32);

	for (size_t i = 0; i < sarr.size; ++i) {
		TestStruct out = {0};
		CHECK(TestStruct_at(&sarr, i, &out));
		CHECK(out.x == (int)i);
		CHECK(out.y == (int)i * 10);
	}
	TestStruct_free(&sarr);

	return true;
}

static bool test_pop(void) {
	int_DynArray iarr;
	int out = 0;

	CHECK(int_init(&iarr));
	for (int i = 0; i < 32; ++i) {
		CHECK(int_push(&iarr, i));
	}

	for (int expected = 31; expected >= 0; --expected) {
		CHECK(int_pop(&iarr, &out));
		CHECK(out == expected);
	}
	CHECK(!int_pop(&iarr, &out));
	CHECK(int_is_empty(&iarr));
	int_free(&iarr);

	TestStruct_DynArray sarr;
	TestStruct sout = {0};

	CHECK(TestStruct_init(&sarr));
	for (int i = 0; i < 32; ++i) {
		CHECK(TestStruct_push(&sarr, (TestStruct){.x = i, .y = i + 100}));
	}

	for (int expected = 31; expected >= 0; --expected) {
		CHECK(TestStruct_pop(&sarr, &sout));
		CHECK(sout.x == expected);
		CHECK(sout.y == expected + 100);
	}
	CHECK(!TestStruct_pop(&sarr, &sout));
	CHECK(TestStruct_is_empty(&sarr));
	TestStruct_free(&sarr);

	return true;
}

static bool test_insert_at(void) {
	int_DynArray iarr;
	CHECK(int_init(&iarr));

	CHECK(int_push(&iarr, 1));
	CHECK(int_push(&iarr, 3));
	CHECK(int_push(&iarr, 4));

	CHECK(int_insert_at(&iarr, 1, 2));
	CHECK(int_insert_at(&iarr, 0, 0));
	CHECK(int_insert_at(&iarr, iarr.size, 5));
	CHECK(!int_insert_at(&iarr, iarr.size + 1, 6));

	for (size_t i = 0; i < iarr.size; ++i) {
		int out = -1;
		CHECK(int_at(&iarr, i, &out));
		CHECK(out == (int)i);
	}

	int_free(&iarr);
	return true;
}

static bool test_erase_at(void) {
	int_DynArray iarr;
	CHECK(int_init(&iarr));

	for (int i = 0; i <= 5; ++i) {
		CHECK(int_push(&iarr, i));
	}

	CHECK(int_erase_at(&iarr, 0));
	CHECK(int_erase_at(&iarr, 2));
	CHECK(int_erase_at(&iarr, iarr.size - 1));
	CHECK(!int_erase_at(&iarr, iarr.size));

	const int expected[] = {1, 2, 4};
	CHECK(iarr.size == 3);

	for (size_t i = 0; i < iarr.size; ++i) {
		int out = -1;
		CHECK(int_at(&iarr, i, &out));
		CHECK(out == expected[i]);
	}

	int_clear(&iarr);
	CHECK(!int_erase_at(&iarr, 0));

	int_free(&iarr);
	return true;
}

static bool test_at(void) {
	int_DynArray iarr;
	CHECK(int_init(&iarr));
	CHECK(int_push(&iarr, 42));

	int out = 7;
	CHECK(int_at(&iarr, 0, &out));
	CHECK(out == 42);
	CHECK(!int_at(&iarr, 1, &out));
	CHECK(out == 42);

	int_free(&iarr);

	TestStruct_DynArray sarr;
	CHECK(TestStruct_init(&sarr));
	CHECK(TestStruct_push(&sarr, (TestStruct){.x = 9, .y = 99}));

	TestStruct sout = {.x = -1, .y = -1};
	CHECK(TestStruct_at(&sarr, 0, &sout));
	CHECK(sout.x == 9 && sout.y == 99);
	CHECK(!TestStruct_at(&sarr, 1, &sout));
	CHECK(sout.x == 9 && sout.y == 99);

	TestStruct_free(&sarr);
	return true;
}

static bool test_find_index_and_find(void) {
	int_DynArray iarr;
	CHECK(int_init(&iarr));
	CHECK(int_push(&iarr, 10));
	CHECK(int_push(&iarr, 20));
	CHECK(int_push(&iarr, 30));

	CHECK(int_find_index(&iarr, cmp_int_lt, 20) == 1);
	CHECK(int_find_index(&iarr, cmp_int_lt, 999) == iarr.size);

	int out = 0;
	CHECK(int_find(&iarr, cmp_int_lt, 30, &out));
	CHECK(out == 30);
	CHECK(!int_find(&iarr, cmp_int_lt, -1, &out));

	int_free(&iarr);

	TestStruct_DynArray sarr;
	CHECK(TestStruct_init(&sarr));
	CHECK(TestStruct_push(&sarr, (TestStruct){.x = 1, .y = 4}));
	CHECK(TestStruct_push(&sarr, (TestStruct){.x = 2, .y = 9}));
	CHECK(TestStruct_push(&sarr, (TestStruct){.x = 3, .y = 7}));

	CHECK(TestStruct_find_index(&sarr, cmp_TestStruct_lt, (TestStruct){.x = 999, .y = 7}) == 2);
	CHECK(TestStruct_find_index(&sarr, cmp_TestStruct_lt, (TestStruct){.x = 0, .y = 11}) == sarr.size);

	TestStruct sout = {0};
	CHECK(TestStruct_find(&sarr, cmp_TestStruct_lt, (TestStruct){.x = 0, .y = 4}, &sout));
	CHECK(sout.x == 1 && sout.y == 4);
	CHECK(!TestStruct_find(&sarr, cmp_TestStruct_lt, (TestStruct){.x = 0, .y = 100}, &sout));

	TestStruct_free(&sarr);
	return true;
}

static bool test_sort(void) {
	int_DynArray iarr;
	CHECK(int_init(&iarr));

	const int unsorted[] = {5, 1, 4, 2, 3};
	for (size_t i = 0; i < 5; ++i) {
		CHECK(int_push(&iarr, unsorted[i]));
	}

	int_sort(&iarr, cmp_int_lt);
	for (size_t i = 0; i < 5; ++i) {
		int out = 0;
		CHECK(int_at(&iarr, i, &out));
		CHECK(out == (int)(i + 1));
	}

	int_sort(&iarr, cmp_int_gt);
	for (size_t i = 0; i < 5; ++i) {
		int out = 0;
		CHECK(int_at(&iarr, i, &out));
		CHECK(out == (int)(5 - i));
	}

	int_free(&iarr);

	TestStruct_DynArray sarr;
	CHECK(TestStruct_init(&sarr));
	CHECK(TestStruct_push(&sarr, (TestStruct){.x = 1, .y = 30}));
	CHECK(TestStruct_push(&sarr, (TestStruct){.x = 2, .y = 10}));
	CHECK(TestStruct_push(&sarr, (TestStruct){.x = 3, .y = 20}));

	TestStruct_sort(&sarr, cmp_TestStruct_lt);
	{
		TestStruct out = {0};
		CHECK(TestStruct_at(&sarr, 0, &out) && out.y == 10);
		CHECK(TestStruct_at(&sarr, 1, &out) && out.y == 20);
		CHECK(TestStruct_at(&sarr, 2, &out) && out.y == 30);
	}

	TestStruct_sort(&sarr, cmp_TestStruct_gt);
	{
		TestStruct out = {0};
		CHECK(TestStruct_at(&sarr, 0, &out) && out.y == 30);
		CHECK(TestStruct_at(&sarr, 1, &out) && out.y == 20);
		CHECK(TestStruct_at(&sarr, 2, &out) && out.y == 10);
	}

	TestStruct_free(&sarr);
	return true;
}

static void run_tests(void) {
	RUN_TEST(test_init);
	RUN_TEST(test_is_full);
	RUN_TEST(test_is_empty);
	RUN_TEST(test_reserve);
	RUN_TEST(test_push);
	RUN_TEST(test_pop);
	RUN_TEST(test_insert_at);
	RUN_TEST(test_erase_at);
	RUN_TEST(test_at);
	RUN_TEST(test_find_index_and_find);
	RUN_TEST(test_sort);
}

int main(void) {
	run_tests();

	if (tests_failed != 0) {
		fprintf(stderr, "\n%d/%d tests failed\n", tests_failed, tests_run);
		return 1;
	}

	fprintf(stdout, "\nAll %d tests passed\n", tests_run);
	return 0;
}
