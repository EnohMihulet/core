#include <stdbool.h>
#include <stdio.h>

#include "test.h"

#define CORE_QUEUE_IMPLEMENTATION
#include "../queue.h"

static int tests_run = 0;
static int tests_failed = 0;

typedef struct TestStruct {
	int x;
	int y;
} TestStruct;

QUEUE_DEFINE(int, int, 4)
QUEUE_DEFINE(TestStruct, TestStruct, 4)

static bool test_init(void) {
	int_Queue iq;
	CHECK(int_init(&iq));
	CHECK(iq.items != NULL);
	CHECK(iq.size == 0);
	CHECK(iq.capacity == 4);
	CHECK(iq.front == 0);
	CHECK(iq.back == 0);
	int_free(&iq);
	CHECK(iq.items == NULL);
	CHECK(iq.size == 0);
	CHECK(iq.capacity == 0);

	TestStruct_Queue sq;
	CHECK(TestStruct_init(&sq));
	CHECK(sq.items != NULL);
	CHECK(sq.size == 0);
	CHECK(sq.capacity == 4);
	CHECK(sq.front == 0);
	CHECK(sq.back == 0);
	TestStruct_free(&sq);
	CHECK(sq.items == NULL);
	CHECK(sq.size == 0);
	CHECK(sq.capacity == 0);

	return true;
}

static bool test_is_full(void) {
	int_Queue iq;
	CHECK(int_init(&iq));
	CHECK(!int_is_full(&iq));

	size_t cap = iq.capacity;
	for (size_t i = 0; i < cap; ++i) {
		CHECK(int_enqueue(&iq, (int)i));
	}
	CHECK(int_is_full(&iq));

	int out = 0;
	CHECK(int_dequeue(&iq, &out));
	CHECK(!int_is_full(&iq));
	int_free(&iq);

	TestStruct_Queue sq;
	CHECK(TestStruct_init(&sq));
	CHECK(!TestStruct_is_full(&sq));

	cap = sq.capacity;
	for (size_t i = 0; i < cap; ++i) {
		CHECK(TestStruct_enqueue(&sq, (TestStruct){.x = (int)i, .y = (int)i}));
	}
	CHECK(TestStruct_is_full(&sq));

	TestStruct sout = {0};
	CHECK(TestStruct_dequeue(&sq, &sout));
	CHECK(!TestStruct_is_full(&sq));
	TestStruct_free(&sq);

	return true;
}

static bool test_is_empty(void) {
	int_Queue iq;
	CHECK(int_init(&iq));
	CHECK(int_is_empty(&iq));
	CHECK(int_enqueue(&iq, 123));
	CHECK(!int_is_empty(&iq));
	int_clear(&iq);
	CHECK(int_is_empty(&iq));
	int out = 0;
	CHECK(!int_dequeue(&iq, &out));
	int_free(&iq);

	TestStruct_Queue sq;
	CHECK(TestStruct_init(&sq));
	CHECK(TestStruct_is_empty(&sq));
	CHECK(TestStruct_enqueue(&sq, (TestStruct){.x = 1, .y = 2}));
	CHECK(!TestStruct_is_empty(&sq));
	TestStruct_clear(&sq);
	CHECK(TestStruct_is_empty(&sq));
	TestStruct sout = {0};
	CHECK(!TestStruct_dequeue(&sq, &sout));
	TestStruct_free(&sq);

	return true;
}

static bool test_reserve(void) {
	int_Queue iq;
	int out = 0;

	CHECK(int_init(&iq));
	CHECK(int_enqueue(&iq, 11));
	CHECK(int_enqueue(&iq, 22));
	CHECK(int_enqueue(&iq, 33));
	CHECK(int_enqueue(&iq, 44));
	CHECK(int_dequeue(&iq, &out) && out == 11);
	CHECK(int_dequeue(&iq, &out) && out == 22);
	CHECK(int_enqueue(&iq, 55));
	CHECK(int_enqueue(&iq, 66));

	const size_t before_cap = iq.capacity;
	CHECK(int_reserve(&iq, before_cap + 13));
	CHECK(iq.capacity >= before_cap + 13);
	CHECK(iq.size == 4);

	const int expected[] = {33, 44, 55, 66};
	for (size_t i = 0; i < 4; ++i) {
		CHECK(int_dequeue(&iq, &out));
		CHECK(out == expected[i]);
	}

	const size_t grown_cap = iq.capacity;
	CHECK(int_reserve(&iq, grown_cap));
	CHECK(int_reserve(&iq, 1));
	CHECK(iq.capacity == grown_cap);

	int_free(&iq);

	TestStruct_Queue sq;
	TestStruct sout = {0};

	CHECK(TestStruct_init(&sq));
	CHECK(TestStruct_enqueue(&sq, (TestStruct){.x = 1, .y = 10}));
	CHECK(TestStruct_enqueue(&sq, (TestStruct){.x = 2, .y = 20}));
	CHECK(TestStruct_enqueue(&sq, (TestStruct){.x = 3, .y = 30}));
	CHECK(TestStruct_dequeue(&sq, &sout));
	CHECK(sout.x == 1 && sout.y == 10);
	CHECK(TestStruct_enqueue(&sq, (TestStruct){.x = 4, .y = 40}));

	const size_t before_struct_cap = sq.capacity;
	CHECK(TestStruct_reserve(&sq, before_struct_cap + 9));
	CHECK(sq.capacity >= before_struct_cap + 9);
	CHECK(sq.size == 3);

	CHECK(TestStruct_dequeue(&sq, &sout));
	CHECK(sout.x == 2 && sout.y == 20);
	CHECK(TestStruct_dequeue(&sq, &sout));
	CHECK(sout.x == 3 && sout.y == 30);
	CHECK(TestStruct_dequeue(&sq, &sout));
	CHECK(sout.x == 4 && sout.y == 40);

	const size_t grown_struct_cap = sq.capacity;
	CHECK(TestStruct_reserve(&sq, grown_struct_cap));
	CHECK(TestStruct_reserve(&sq, 1));
	CHECK(sq.capacity == grown_struct_cap);

	TestStruct_free(&sq);

	return true;
}

static bool test_enqueue_and_dequeue(void) {
	int_Queue iq;
	int out = 0;

	CHECK(int_init(&iq));
	for (int i = 0; i < 32; ++i) {
		CHECK(int_enqueue(&iq, i));
	}
	CHECK(iq.size == 32);

	for (int expected = 0; expected < 32; ++expected) {
		CHECK(int_dequeue(&iq, &out));
		CHECK(out == expected);
	}
	CHECK(!int_dequeue(&iq, &out));
	CHECK(int_is_empty(&iq));
	int_free(&iq);

	TestStruct_Queue sq;
	TestStruct sout = {0};

	CHECK(TestStruct_init(&sq));
	for (int i = 0; i < 32; ++i) {
		CHECK(TestStruct_enqueue(&sq, (TestStruct){.x = i, .y = i + 100}));
	}

	for (int expected = 0; expected < 32; ++expected) {
		CHECK(TestStruct_dequeue(&sq, &sout));
		CHECK(sout.x == expected);
		CHECK(sout.y == expected + 100);
	}
	CHECK(!TestStruct_dequeue(&sq, &sout));
	CHECK(TestStruct_is_empty(&sq));
	TestStruct_free(&sq);

	return true;
}

static bool test_front(void) {
	int_Queue iq;
	CHECK(int_init(&iq));

	int out = -1;
	CHECK(!int_front(&iq, &out));

	CHECK(int_enqueue(&iq, 7));
	CHECK(int_front(&iq, &out));
	CHECK(out == 7);

	CHECK(int_enqueue(&iq, 9));
	CHECK(int_front(&iq, &out));
	CHECK(out == 7);

	CHECK(int_dequeue(&iq, &out));
	CHECK(out == 7);
	CHECK(int_front(&iq, &out));
	CHECK(out == 9);

	int_free(&iq);

	TestStruct_Queue sq;
	CHECK(TestStruct_init(&sq));

	TestStruct sout = {0};
	CHECK(!TestStruct_front(&sq, &sout));

	CHECK(TestStruct_enqueue(&sq, (TestStruct){.x = 1, .y = 11}));
	CHECK(TestStruct_enqueue(&sq, (TestStruct){.x = 2, .y = 22}));
	CHECK(TestStruct_front(&sq, &sout));
	CHECK(sout.x == 1 && sout.y == 11);

	CHECK(TestStruct_dequeue(&sq, &sout));
	CHECK(TestStruct_front(&sq, &sout));
	CHECK(sout.x == 2 && sout.y == 22);

	TestStruct_free(&sq);
	return true;
}

static bool test_wraparound(void) {
	int_Queue iq;
	CHECK(int_init(&iq));

	CHECK(int_enqueue(&iq, 0));
	CHECK(int_enqueue(&iq, 1));
	CHECK(int_enqueue(&iq, 2));
	CHECK(int_enqueue(&iq, 3));

	int out = 0;
	CHECK(int_dequeue(&iq, &out) && out == 0);
	CHECK(int_dequeue(&iq, &out) && out == 1);

	CHECK(int_enqueue(&iq, 4));
	CHECK(int_enqueue(&iq, 5));

	const int expected[] = {2, 3, 4, 5};
	for (size_t i = 0; i < 4; ++i) {
		CHECK(int_dequeue(&iq, &out));
		CHECK(out == expected[i]);
	}
	CHECK(int_is_empty(&iq));

	int_free(&iq);
	return true;
}

static void run_tests(void) {
	RUN_TEST(test_init);
	RUN_TEST(test_is_full);
	RUN_TEST(test_is_empty);
	RUN_TEST(test_reserve);
	RUN_TEST(test_enqueue_and_dequeue);
	RUN_TEST(test_front);
	RUN_TEST(test_wraparound);
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
