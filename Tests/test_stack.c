#include <stdbool.h>
#include <stdio.h>

#include "test.h"

#define CORE_STACK_IMPLEMENTATION
#include "../DataStructures/stack.h"

static int tests_run = 0;
static int tests_failed = 0;

typedef struct TestStruct {
	int x;
	int y;
} TestStruct;

STACK_DEFINE(int, int, 4)
STACK_DEFINE(TestStruct, TestStruct, 4)

static bool test_init(void) {
	int_Stack ist;
	CHECK(int_init(&ist));
	CHECK(ist.items != NULL);
	CHECK(ist.size == 0);
	CHECK(ist.capacity == 4);
	int_free(&ist);
	CHECK(ist.items == NULL);
	CHECK(ist.size == 0);
	CHECK(ist.capacity == 0);

	TestStruct_Stack sst;
	CHECK(TestStruct_init(&sst));
	CHECK(sst.items != NULL);
	CHECK(sst.size == 0);
	CHECK(sst.capacity == 4);
	TestStruct_free(&sst);
	CHECK(sst.items == NULL);
	CHECK(sst.size == 0);
	CHECK(sst.capacity == 0);

	return true;
}

static bool test_is_full(void) {
	int_Stack ist;
	CHECK(int_init(&ist));
	CHECK(!int_is_full(&ist));

	size_t cap = ist.capacity;
	for (size_t i = 0; i < cap; ++i) {
		CHECK(int_push(&ist, (int)i));
	}
	CHECK(int_is_full(&ist));

	int out = 0;
	CHECK(int_pop(&ist, &out));
	CHECK(!int_is_full(&ist));
	int_free(&ist);

	TestStruct_Stack sst;
	CHECK(TestStruct_init(&sst));
	CHECK(!TestStruct_is_full(&sst));

	cap = sst.capacity;
	for (size_t i = 0; i < cap; ++i) {
		CHECK(TestStruct_push(&sst, (TestStruct){.x = (int)i, .y = (int)i}));
	}
	CHECK(TestStruct_is_full(&sst));

	TestStruct sout = {0};
	CHECK(TestStruct_pop(&sst, &sout));
	CHECK(!TestStruct_is_full(&sst));
	TestStruct_free(&sst);

	return true;
}

static bool test_is_empty(void) {
	int_Stack ist;
	CHECK(int_init(&ist));
	CHECK(int_is_empty(&ist));
	CHECK(int_push(&ist, 123));
	CHECK(!int_is_empty(&ist));
	int_clear(&ist);
	CHECK(int_is_empty(&ist));
	int out = 0;
	CHECK(!int_pop(&ist, &out));
	int_free(&ist);

	TestStruct_Stack sst;
	CHECK(TestStruct_init(&sst));
	CHECK(TestStruct_is_empty(&sst));
	CHECK(TestStruct_push(&sst, (TestStruct){.x = 1, .y = 2}));
	CHECK(!TestStruct_is_empty(&sst));
	TestStruct_clear(&sst);
	CHECK(TestStruct_is_empty(&sst));
	TestStruct sout = {0};
	CHECK(!TestStruct_pop(&sst, &sout));
	TestStruct_free(&sst);

	return true;
}

static bool test_reserve(void) {
	int_Stack ist;
	int out = 0;

	CHECK(int_init(&ist));
	CHECK(int_push(&ist, 11));
	CHECK(int_push(&ist, 22));
	CHECK(int_push(&ist, 33));

	const size_t before_cap = ist.capacity;
	CHECK(int_reserve(&ist, before_cap + 13));
	CHECK(ist.capacity >= before_cap + 13);
	CHECK(ist.size == 3);

	CHECK(int_top(&ist, &out));
	CHECK(out == 33);
	CHECK(int_pop(&ist, &out) && out == 33);
	CHECK(int_pop(&ist, &out) && out == 22);
	CHECK(int_pop(&ist, &out) && out == 11);

	const size_t grown_cap = ist.capacity;
	CHECK(int_reserve(&ist, grown_cap));
	CHECK(int_reserve(&ist, 1));
	CHECK(ist.capacity == grown_cap);

	int_free(&ist);

	TestStruct_Stack sst;
	TestStruct sout = {0};

	CHECK(TestStruct_init(&sst));
	CHECK(TestStruct_push(&sst, (TestStruct){.x = 10, .y = 100}));
	CHECK(TestStruct_push(&sst, (TestStruct){.x = 20, .y = 200}));

	const size_t before_struct_cap = sst.capacity;
	CHECK(TestStruct_reserve(&sst, before_struct_cap + 9));
	CHECK(sst.capacity >= before_struct_cap + 9);
	CHECK(sst.size == 2);

	CHECK(TestStruct_top(&sst, &sout));
	CHECK(sout.x == 20 && sout.y == 200);
	CHECK(TestStruct_pop(&sst, &sout));
	CHECK(sout.x == 20 && sout.y == 200);
	CHECK(TestStruct_pop(&sst, &sout));
	CHECK(sout.x == 10 && sout.y == 100);

	const size_t grown_struct_cap = sst.capacity;
	CHECK(TestStruct_reserve(&sst, grown_struct_cap));
	CHECK(TestStruct_reserve(&sst, 1));
	CHECK(sst.capacity == grown_struct_cap);

	TestStruct_free(&sst);

	return true;
}

static bool test_push(void) {
	int_Stack ist;
	CHECK(int_init(&ist));

	for (int i = 0; i < 32; ++i) {
		CHECK(int_push(&ist, i));
	}
	CHECK(ist.size == 32);

	int out = -1;
	CHECK(int_top(&ist, &out));
	CHECK(out == 31);
	int_free(&ist);

	TestStruct_Stack sst;
	CHECK(TestStruct_init(&sst));

	for (int i = 0; i < 32; ++i) {
		CHECK(TestStruct_push(&sst, (TestStruct){.x = i, .y = i * 10}));
	}
	CHECK(sst.size == 32);

	TestStruct sout = {0};
	CHECK(TestStruct_top(&sst, &sout));
	CHECK(sout.x == 31);
	CHECK(sout.y == 310);
	TestStruct_free(&sst);

	return true;
}

static bool test_pop(void) {
	int_Stack ist;
	int out = 0;

	CHECK(int_init(&ist));
	for (int i = 0; i < 32; ++i) {
		CHECK(int_push(&ist, i));
	}

	for (int expected = 31; expected >= 0; --expected) {
		CHECK(int_pop(&ist, &out));
		CHECK(out == expected);
	}
	CHECK(!int_pop(&ist, &out));
	CHECK(int_is_empty(&ist));
	int_free(&ist);

	TestStruct_Stack sst;
	TestStruct sout = {0};

	CHECK(TestStruct_init(&sst));
	for (int i = 0; i < 32; ++i) {
		CHECK(TestStruct_push(&sst, (TestStruct){.x = i, .y = i + 100}));
	}

	for (int expected = 31; expected >= 0; --expected) {
		CHECK(TestStruct_pop(&sst, &sout));
		CHECK(sout.x == expected);
		CHECK(sout.y == expected + 100);
	}
	CHECK(!TestStruct_pop(&sst, &sout));
	CHECK(TestStruct_is_empty(&sst));
	TestStruct_free(&sst);

	return true;
}

static bool test_top(void) {
	int_Stack ist;
	CHECK(int_init(&ist));

	int out = -1;
	CHECK(!int_top(&ist, &out));

	CHECK(int_push(&ist, 7));
	CHECK(int_top(&ist, &out));
	CHECK(out == 7);

	CHECK(int_push(&ist, 9));
	CHECK(int_top(&ist, &out));
	CHECK(out == 9);

	CHECK(int_pop(&ist, &out));
	CHECK(out == 9);
	CHECK(int_top(&ist, &out));
	CHECK(out == 7);

	int_free(&ist);

	TestStruct_Stack sst;
	CHECK(TestStruct_init(&sst));

	TestStruct sout = {0};
	CHECK(!TestStruct_top(&sst, &sout));

	CHECK(TestStruct_push(&sst, (TestStruct){.x = 1, .y = 11}));
	CHECK(TestStruct_push(&sst, (TestStruct){.x = 2, .y = 22}));
	CHECK(TestStruct_top(&sst, &sout));
	CHECK(sout.x == 2 && sout.y == 22);

	CHECK(TestStruct_pop(&sst, &sout));
	CHECK(TestStruct_top(&sst, &sout));
	CHECK(sout.x == 1 && sout.y == 11);

	TestStruct_free(&sst);
	return true;
}

static void run_tests(void) {
	RUN_TEST(test_init);
	RUN_TEST(test_is_full);
	RUN_TEST(test_is_empty);
	RUN_TEST(test_reserve);
	RUN_TEST(test_push);
	RUN_TEST(test_pop);
	RUN_TEST(test_top);
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
