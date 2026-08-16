#include <stdbool.h>
#include <stdio.h>

#include "test.h"

#define CORE_RINGBUFFER_IMPLEMENTATION
#include "../ringbuffer.h"

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

RINGBUFFER_DEFINE(int, int, 4)
RINGBUFFER_DEFINE(TestStruct, TestStruct, 4)

static bool test_init(void) {
	int_RingBuffer irb;
	CHECK(int_init(&irb));
	CHECK(irb.items != NULL);
	CHECK(irb.size == 0);
	CHECK(irb.capacity == 4);
	CHECK(irb.front == 0);
	CHECK(irb.back == 0);
	int_free(&irb);
	CHECK(irb.items == NULL);
	CHECK(irb.size == 0);
	CHECK(irb.capacity == 0);

	TestStruct_RingBuffer srb;
	CHECK(TestStruct_init(&srb));
	CHECK(srb.items != NULL);
	CHECK(srb.size == 0);
	CHECK(srb.capacity == 4);
	CHECK(srb.front == 0);
	CHECK(srb.back == 0);
	TestStruct_free(&srb);
	CHECK(srb.items == NULL);
	CHECK(srb.size == 0);
	CHECK(srb.capacity == 0);

	return true;
}

static bool test_is_full(void) {
	int_RingBuffer irb;
	CHECK(int_init(&irb));
	CHECK(!int_is_full(&irb));

	size_t cap = irb.capacity;
	for (size_t i = 0; i < cap; ++i) {
		CHECK(int_push_back(&irb, (int)i));
	}
	CHECK(int_is_full(&irb));

	int out = 0;
	CHECK(int_pop_front(&irb, &out));
	CHECK(!int_is_full(&irb));
	int_free(&irb);

	TestStruct_RingBuffer srb;
	CHECK(TestStruct_init(&srb));
	CHECK(!TestStruct_is_full(&srb));

	cap = srb.capacity;
	for (size_t i = 0; i < cap; ++i) {
		CHECK(TestStruct_push_front(&srb, (TestStruct){.x = (int)i, .y = (int)i}));
	}
	CHECK(TestStruct_is_full(&srb));

	TestStruct sout = {0};
	CHECK(TestStruct_pop_back(&srb, &sout));
	CHECK(!TestStruct_is_full(&srb));
	TestStruct_free(&srb);

	return true;
}

static bool test_is_empty(void) {
	int_RingBuffer irb;
	CHECK(int_init(&irb));
	CHECK(int_is_empty(&irb));
	CHECK(int_push_back(&irb, 123));
	CHECK(!int_is_empty(&irb));
	int_clear(&irb);
	CHECK(int_is_empty(&irb));
	int out = 0;
	CHECK(!int_pop_back(&irb, &out));
	int_free(&irb);

	TestStruct_RingBuffer srb;
	CHECK(TestStruct_init(&srb));
	CHECK(TestStruct_is_empty(&srb));
	CHECK(TestStruct_push_front(&srb, (TestStruct){.x = 1, .y = 2}));
	CHECK(!TestStruct_is_empty(&srb));
	TestStruct_clear(&srb);
	CHECK(TestStruct_is_empty(&srb));
	TestStruct sout = {0};
	CHECK(!TestStruct_pop_front(&srb, &sout));
	TestStruct_free(&srb);

	return true;
}

static bool test_reserve(void) {
	int_RingBuffer irb;
	int out = 0;

	CHECK(int_init(&irb));
	CHECK(int_push_back(&irb, 1));
	CHECK(int_push_back(&irb, 2));
	CHECK(int_push_back(&irb, 3));
	CHECK(int_push_back(&irb, 4));
	CHECK(int_pop_front(&irb, &out) && out == 1);
	CHECK(int_pop_front(&irb, &out) && out == 2);
	CHECK(int_push_back(&irb, 5));
	CHECK(int_push_back(&irb, 6));

	const size_t before_cap = irb.capacity;
	CHECK(int_reserve(&irb, before_cap + 13));
	CHECK(irb.capacity >= before_cap + 13);
	CHECK(irb.size == 4);

	const int expected[] = {3, 4, 5, 6};
	for (size_t i = 0; i < 4; ++i) {
		CHECK(int_at(&irb, i, &out));
		CHECK(out == expected[i]);
	}

	const size_t grown_cap = irb.capacity;
	CHECK(int_reserve(&irb, grown_cap));
	CHECK(int_reserve(&irb, 1));
	CHECK(irb.capacity == grown_cap);

	int_free(&irb);

	TestStruct_RingBuffer srb;
	TestStruct sout = {0};

	CHECK(TestStruct_init(&srb));
	CHECK(TestStruct_push_back(&srb, (TestStruct){.x = 1, .y = 10}));
	CHECK(TestStruct_push_back(&srb, (TestStruct){.x = 2, .y = 20}));
	CHECK(TestStruct_push_back(&srb, (TestStruct){.x = 3, .y = 30}));
	CHECK(TestStruct_push_back(&srb, (TestStruct){.x = 4, .y = 40}));
	CHECK(TestStruct_pop_front(&srb, &sout));
	CHECK(sout.x == 1 && sout.y == 10);
	CHECK(TestStruct_push_back(&srb, (TestStruct){.x = 5, .y = 50}));

	const size_t before_struct_cap = srb.capacity;
	CHECK(TestStruct_reserve(&srb, before_struct_cap + 9));
	CHECK(srb.capacity >= before_struct_cap + 9);
	CHECK(srb.size == 4);

	CHECK(TestStruct_at(&srb, 0, &sout));
	CHECK(sout.x == 2 && sout.y == 20);
	CHECK(TestStruct_at(&srb, 1, &sout));
	CHECK(sout.x == 3 && sout.y == 30);
	CHECK(TestStruct_at(&srb, 2, &sout));
	CHECK(sout.x == 4 && sout.y == 40);
	CHECK(TestStruct_at(&srb, 3, &sout));
	CHECK(sout.x == 5 && sout.y == 50);

	const size_t grown_struct_cap = srb.capacity;
	CHECK(TestStruct_reserve(&srb, grown_struct_cap));
	CHECK(TestStruct_reserve(&srb, 1));
	CHECK(srb.capacity == grown_struct_cap);

	TestStruct_free(&srb);

	return true;
}

static bool test_push_and_pop(void) {
	int_RingBuffer irb;
	int out = 0;

	CHECK(int_init(&irb));
	for (int i = 0; i < 32; ++i) {
		CHECK(int_push_back(&irb, i));
	}

	for (int expected = 0; expected < 32; ++expected) {
		CHECK(int_pop_front(&irb, &out));
		CHECK(out == expected);
	}
	CHECK(!int_pop_front(&irb, &out));
	CHECK(int_is_empty(&irb));

	for (int i = 0; i < 32; ++i) {
		CHECK(int_push_front(&irb, i));
	}

	for (int expected = 0; expected < 32; ++expected) {
		CHECK(int_pop_back(&irb, &out));
		CHECK(out == expected);
	}
	CHECK(!int_pop_back(&irb, &out));
	CHECK(int_is_empty(&irb));
	int_free(&irb);

	TestStruct_RingBuffer srb;
	TestStruct sout = {0};

	CHECK(TestStruct_init(&srb));
	for (int i = 0; i < 32; ++i) {
		CHECK(TestStruct_push_front(&srb, (TestStruct){.x = i, .y = i + 100}));
	}

	for (int expected = 0; expected < 32; ++expected) {
		CHECK(TestStruct_pop_back(&srb, &sout));
		CHECK(sout.x == expected);
		CHECK(sout.y == expected + 100);
	}
	CHECK(!TestStruct_pop_front(&srb, &sout));
	CHECK(TestStruct_is_empty(&srb));
	TestStruct_free(&srb);

	return true;
}

static bool test_insert_at(void) {
	int_RingBuffer irb;
	CHECK(int_init(&irb));

	CHECK(int_push_back(&irb, 1));
	CHECK(int_push_back(&irb, 3));
	CHECK(int_push_back(&irb, 4));

	CHECK(int_insert_at(&irb, 1, 2));
	CHECK(int_insert_at(&irb, 0, 0));
	CHECK(int_insert_at(&irb, irb.size, 5));
	CHECK(!int_insert_at(&irb, irb.size + 1, 6));

	for (size_t i = 0; i < irb.size; ++i) {
		int out = -1;
		CHECK(int_at(&irb, i, &out));
		CHECK(out == (int)i);
	}

	int_clear(&irb);
	CHECK(int_push_back(&irb, 0));
	CHECK(int_push_back(&irb, 1));
	CHECK(int_push_back(&irb, 2));
	CHECK(int_push_back(&irb, 3));

	int out = 0;
	CHECK(int_pop_front(&irb, &out) && out == 0);
	CHECK(int_push_back(&irb, 4));
	CHECK(int_insert_at(&irb, 2, 99));

	const int expected_wrapped[] = {1, 2, 99, 3, 4};
	CHECK(irb.size == 5);
	for (size_t i = 0; i < irb.size; ++i) {
		CHECK(int_at(&irb, i, &out));
		CHECK(out == expected_wrapped[i]);
	}

	int_free(&irb);
	return true;
}

static bool test_erase_at(void) {
	int_RingBuffer irb;
	CHECK(int_init(&irb));

	for (int i = 0; i <= 5; ++i) {
		CHECK(int_push_back(&irb, i));
	}

	CHECK(int_erase_at(&irb, 0));
	CHECK(int_erase_at(&irb, 2));
	CHECK(int_erase_at(&irb, irb.size - 1));
	CHECK(!int_erase_at(&irb, irb.size));

	const int expected[] = {1, 2, 4};
	CHECK(irb.size == 3);

	for (size_t i = 0; i < irb.size; ++i) {
		int out = -1;
		CHECK(int_at(&irb, i, &out));
		CHECK(out == expected[i]);
	}

	int_clear(&irb);
	CHECK(!int_erase_at(&irb, 0));

	int_free(&irb);
	return true;
}

static bool test_at(void) {
	int_RingBuffer irb;
	CHECK(int_init(&irb));
	CHECK(int_push_back(&irb, 42));

	int out = 7;
	CHECK(int_at(&irb, 0, &out));
	CHECK(out == 42);
	CHECK(!int_at(&irb, 1, &out));
	CHECK(out == 42);

	int_free(&irb);

	TestStruct_RingBuffer srb;
	CHECK(TestStruct_init(&srb));
	CHECK(TestStruct_push_back(&srb, (TestStruct){.x = 9, .y = 99}));

	TestStruct sout = {.x = -1, .y = -1};
	CHECK(TestStruct_at(&srb, 0, &sout));
	CHECK(sout.x == 9 && sout.y == 99);
	CHECK(!TestStruct_at(&srb, 1, &sout));
	CHECK(sout.x == 9 && sout.y == 99);

	TestStruct_free(&srb);
	return true;
}

static bool test_find_index_and_find(void) {
	int_RingBuffer irb;
	CHECK(int_init(&irb));
	CHECK(int_push_back(&irb, 10));
	CHECK(int_push_back(&irb, 20));
	CHECK(int_push_back(&irb, 30));

	CHECK(int_find_index(&irb, cmp_int_lt, 20) == 1);
	CHECK(int_find_index(&irb, cmp_int_lt, 999) == irb.size);

	int out = 0;
	CHECK(int_find(&irb, cmp_int_lt, 30, &out));
	CHECK(out == 30);
	CHECK(!int_find(&irb, cmp_int_lt, -1, &out));

	int_free(&irb);

	TestStruct_RingBuffer srb;
	CHECK(TestStruct_init(&srb));
	CHECK(TestStruct_push_back(&srb, (TestStruct){.x = 1, .y = 4}));
	CHECK(TestStruct_push_back(&srb, (TestStruct){.x = 2, .y = 9}));
	CHECK(TestStruct_push_back(&srb, (TestStruct){.x = 3, .y = 7}));

	CHECK(TestStruct_find_index(&srb, cmp_TestStruct_lt, (TestStruct){.x = 999, .y = 7}) == 2);
	CHECK(TestStruct_find_index(&srb, cmp_TestStruct_lt, (TestStruct){.x = 0, .y = 11}) == srb.size);

	TestStruct sout = {0};
	CHECK(TestStruct_find(&srb, cmp_TestStruct_lt, (TestStruct){.x = 0, .y = 4}, &sout));
	CHECK(sout.x == 1 && sout.y == 4);
	CHECK(!TestStruct_find(&srb, cmp_TestStruct_lt, (TestStruct){.x = 0, .y = 100}, &sout));

	TestStruct_free(&srb);
	return true;
}

static bool test_sort(void) {
	int_RingBuffer irb;
	CHECK(int_init(&irb));

	CHECK(int_push_back(&irb, 3));
	CHECK(int_push_back(&irb, 1));
	CHECK(int_push_back(&irb, 4));
	CHECK(int_push_back(&irb, 2));

	int out = 0;
	CHECK(int_pop_front(&irb, &out) && out == 3);
	CHECK(int_push_back(&irb, 0));

	int_sort(&irb, cmp_int_lt);
	const int expected_asc[] = {0, 1, 2, 4};
	for (size_t i = 0; i < 4; ++i) {
		CHECK(int_at(&irb, i, &out));
		CHECK(out == expected_asc[i]);
	}

	int_sort(&irb, cmp_int_gt);
	const int expected_desc[] = {4, 2, 1, 0};
	for (size_t i = 0; i < 4; ++i) {
		CHECK(int_at(&irb, i, &out));
		CHECK(out == expected_desc[i]);
	}

	int_free(&irb);

	TestStruct_RingBuffer srb;
	CHECK(TestStruct_init(&srb));
	CHECK(TestStruct_push_back(&srb, (TestStruct){.x = 1, .y = 30}));
	CHECK(TestStruct_push_back(&srb, (TestStruct){.x = 2, .y = 10}));
	CHECK(TestStruct_push_back(&srb, (TestStruct){.x = 3, .y = 20}));
	CHECK(TestStruct_push_back(&srb, (TestStruct){.x = 4, .y = 40}));

	TestStruct sout = {0};
	CHECK(TestStruct_pop_front(&srb, &sout));
	CHECK(TestStruct_push_back(&srb, (TestStruct){.x = 5, .y = 25}));

	TestStruct_sort(&srb, cmp_TestStruct_lt);
	CHECK(TestStruct_at(&srb, 0, &sout) && sout.y == 10);
	CHECK(TestStruct_at(&srb, 1, &sout) && sout.y == 20);
	CHECK(TestStruct_at(&srb, 2, &sout) && sout.y == 25);
	CHECK(TestStruct_at(&srb, 3, &sout) && sout.y == 40);

	TestStruct_sort(&srb, cmp_TestStruct_gt);
	CHECK(TestStruct_at(&srb, 0, &sout) && sout.y == 40);
	CHECK(TestStruct_at(&srb, 1, &sout) && sout.y == 25);
	CHECK(TestStruct_at(&srb, 2, &sout) && sout.y == 20);
	CHECK(TestStruct_at(&srb, 3, &sout) && sout.y == 10);

	TestStruct_free(&srb);
	return true;
}

static void run_tests(void) {
	RUN_TEST(test_init);
	RUN_TEST(test_is_full);
	RUN_TEST(test_is_empty);
	RUN_TEST(test_reserve);
	RUN_TEST(test_push_and_pop);
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
