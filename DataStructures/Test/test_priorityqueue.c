#include <stdbool.h>
#include <stdio.h>

#include "test.h"

#define CORE_PRIORITYQUEUE_IMPLEMENTATION
#include "../priorityqueue.h"

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

PRIORITYQUEUE_DEFINE(int, int, 4)
PRIORITYQUEUE_DEFINE(TestStruct, TestStruct, 4)

static bool test_init(void) {
	int_PrioQueue ipq;
	CHECK(int_init(&ipq, &cmp_int_gt));
	CHECK(ipq.items != NULL);
	CHECK(ipq.size == 0);
	CHECK(ipq.capacity == 4);
	CHECK(ipq.cmp == &cmp_int_gt);
	int_free(&ipq);
	CHECK(ipq.items == NULL);
	CHECK(ipq.size == 0);
	CHECK(ipq.capacity == 0);
	CHECK(ipq.cmp == NULL);

	TestStruct_PrioQueue spq;
	CHECK(TestStruct_init(&spq, &cmp_TestStruct_gt));
	CHECK(spq.items != NULL);
	CHECK(spq.size == 0);
	CHECK(spq.capacity == 4);
	CHECK(spq.cmp == &cmp_TestStruct_gt);
	TestStruct_free(&spq);
	CHECK(spq.items == NULL);
	CHECK(spq.size == 0);
	CHECK(spq.capacity == 0);
	CHECK(spq.cmp == NULL);

	return true;
}

static bool test_is_full(void) {
	int_PrioQueue ipq;
	CHECK(int_init(&ipq, &cmp_int_gt));
	CHECK(!int_is_full(&ipq));

	size_t cap = ipq.capacity;
	for (size_t i = 0; i < cap; ++i) {
		CHECK(int_insert(&ipq, (int)i));
	}
	CHECK(int_is_full(&ipq));

	int out = 0;
	CHECK(int_extract(&ipq, &out));
	CHECK(!int_is_full(&ipq));
	int_free(&ipq);

	TestStruct_PrioQueue spq;
	CHECK(TestStruct_init(&spq, &cmp_TestStruct_gt));
	CHECK(!TestStruct_is_full(&spq));

	cap = spq.capacity;
	for (size_t i = 0; i < cap; ++i) {
		CHECK(TestStruct_insert(&spq, (TestStruct){.x = (int)i, .y = (int)i}));
	}
	CHECK(TestStruct_is_full(&spq));

	TestStruct sout = {0};
	CHECK(TestStruct_extract(&spq, &sout));
	CHECK(!TestStruct_is_full(&spq));
	TestStruct_free(&spq);

	return true;
}

static bool test_is_empty(void) {
	int_PrioQueue ipq;
	CHECK(int_init(&ipq, &cmp_int_gt));
	CHECK(int_is_empty(&ipq));
	CHECK(int_insert(&ipq, 123));
	CHECK(!int_is_empty(&ipq));
	int_clear(&ipq);
	CHECK(int_is_empty(&ipq));
	int out = 0;
	CHECK(!int_extract(&ipq, &out));
	int_free(&ipq);

	TestStruct_PrioQueue spq;
	CHECK(TestStruct_init(&spq, &cmp_TestStruct_gt));
	CHECK(TestStruct_is_empty(&spq));
	CHECK(TestStruct_insert(&spq, (TestStruct){.x = 1, .y = 2}));
	CHECK(!TestStruct_is_empty(&spq));
	TestStruct_clear(&spq);
	CHECK(TestStruct_is_empty(&spq));
	TestStruct sout = {0};
	CHECK(!TestStruct_extract(&spq, &sout));
	TestStruct_free(&spq);

	return true;
}

static bool test_reserve(void) {
	int_PrioQueue ipq;
	int out = 0;

	CHECK(int_init(&ipq, &cmp_int_gt));
	CHECK(int_insert(&ipq, 11));
	CHECK(int_insert(&ipq, 22));
	CHECK(int_insert(&ipq, 33));

	const size_t before_cap = ipq.capacity;
	CHECK(int_reserve(&ipq, before_cap + 13));
	CHECK(ipq.capacity >= before_cap + 13);
	CHECK(ipq.size == 3);

	bool saw_11 = false;
	bool saw_22 = false;
	bool saw_33 = false;
	for (size_t i = 0; i < 3; ++i) {
		CHECK(int_extract(&ipq, &out));
		switch (out) {
			case 11:
				CHECK(!saw_11);
				saw_11 = true;
				break;
			case 22:
				CHECK(!saw_22);
				saw_22 = true;
				break;
			case 33:
				CHECK(!saw_33);
				saw_33 = true;
				break;
			default:
				CHECK(false);
		}
	}
	CHECK(saw_11 && saw_22 && saw_33);

	const size_t grown_cap = ipq.capacity;
	CHECK(int_reserve(&ipq, grown_cap));
	CHECK(int_reserve(&ipq, 1));
	CHECK(ipq.capacity == grown_cap);

	int_free(&ipq);

	TestStruct_PrioQueue spq;
	TestStruct sout = {0};

	CHECK(TestStruct_init(&spq, &cmp_TestStruct_gt));
	CHECK(TestStruct_insert(&spq, (TestStruct){.x = 10, .y = 100}));
	CHECK(TestStruct_insert(&spq, (TestStruct){.x = 20, .y = 200}));

	const size_t before_struct_cap = spq.capacity;
	CHECK(TestStruct_reserve(&spq, before_struct_cap + 9));
	CHECK(spq.capacity >= before_struct_cap + 9);
	CHECK(spq.size == 2);

	bool saw_10_100 = false;
	bool saw_20_200 = false;
	for (size_t i = 0; i < 2; ++i) {
		CHECK(TestStruct_extract(&spq, &sout));
		if (sout.x == 10 && sout.y == 100) {
			CHECK(!saw_10_100);
			saw_10_100 = true;
		} else if (sout.x == 20 && sout.y == 200) {
			CHECK(!saw_20_200);
			saw_20_200 = true;
		} else {
			CHECK(false);
		}
	}
	CHECK(saw_10_100 && saw_20_200);

	const size_t grown_struct_cap = spq.capacity;
	CHECK(TestStruct_reserve(&spq, grown_struct_cap));
	CHECK(TestStruct_reserve(&spq, 1));
	CHECK(spq.capacity == grown_struct_cap);

	TestStruct_free(&spq);

	return true;
}

static bool test_insert(void) {
	int_PrioQueue ipq;
	CHECK(int_init(&ipq, &cmp_int_gt));

	for (int i = 0; i < 32; ++i) {
		CHECK(int_insert(&ipq, i));
	}
	CHECK(ipq.size == 32);

	int out = -1;
	CHECK(int_extract(&ipq, &out));
	CHECK(out == 31);
	CHECK(ipq.size == 31);
	int_free(&ipq);

	CHECK(int_init(&ipq, &cmp_int_lt));
	for (int i = 0; i < 32; ++i) {
		CHECK(int_insert(&ipq, i));
	}
	CHECK(ipq.size == 32);

	CHECK(int_extract(&ipq, &out));
	CHECK(out == 0);
	CHECK(ipq.size == 31);
	int_free(&ipq);

	TestStruct_PrioQueue spq;
	CHECK(TestStruct_init(&spq, &cmp_TestStruct_gt));

	for (int i = 0; i < 32; ++i) {
		CHECK(TestStruct_insert(&spq, (TestStruct){.x = i, .y = i * 10}));
	}
	CHECK(spq.size == 32);

	TestStruct sout = {0};
	CHECK(TestStruct_extract(&spq, &sout));
	CHECK(sout.x == 31);
	CHECK(sout.y == 310);
	TestStruct_free(&spq);

	CHECK(TestStruct_init(&spq, &cmp_TestStruct_lt));
	for (int i = 0; i < 32; ++i) {
		CHECK(TestStruct_insert(&spq, (TestStruct){.x = i, .y = i * 10}));
	}
	CHECK(spq.size == 32);

	CHECK(TestStruct_extract(&spq, &sout));
	CHECK(sout.x == 0);
	CHECK(sout.y == 0);
	TestStruct_free(&spq);

	return true;
}

static bool test_extract(void) {
	int_PrioQueue ipq;
	int out = 0;

	CHECK(int_init(&ipq, &cmp_int_gt));
	for (int i = 0; i < 32; ++i) {
		CHECK(int_insert(&ipq, i));
	}

	for (int expected = 31; expected >= 0; --expected) {
		CHECK(int_extract(&ipq, &out));
		CHECK(out == expected);
	}
	CHECK(!int_extract(&ipq, &out));
	CHECK(int_is_empty(&ipq));
	int_free(&ipq);

	CHECK(int_init(&ipq, &cmp_int_lt));
	for (int i = 0; i < 32; ++i) {
		CHECK(int_insert(&ipq, i));
	}

	for (int expected = 0; expected < 32; ++expected) {
		CHECK(int_extract(&ipq, &out));
		CHECK(out == expected);
	}
	CHECK(!int_extract(&ipq, &out));
	CHECK(int_is_empty(&ipq));
	int_free(&ipq);

	TestStruct_PrioQueue spq;
	TestStruct sout = {0};

	CHECK(TestStruct_init(&spq, &cmp_TestStruct_gt));
	for (int i = 0; i < 32; ++i) {
		CHECK(TestStruct_insert(&spq, (TestStruct){.x = i, .y = i + 100}));
	}

	for (int expected = 31; expected >= 0; --expected) {
		CHECK(TestStruct_extract(&spq, &sout));
		CHECK(sout.x == expected);
		CHECK(sout.y == expected + 100);
	}
	CHECK(!TestStruct_extract(&spq, &sout));
	CHECK(TestStruct_is_empty(&spq));
	TestStruct_free(&spq);

	CHECK(TestStruct_init(&spq, &cmp_TestStruct_lt));
	for (int i = 0; i < 32; ++i) {
		CHECK(TestStruct_insert(&spq, (TestStruct){.x = i, .y = i + 100}));
	}

	for (int expected = 0; expected < 32; ++expected) {
		CHECK(TestStruct_extract(&spq, &sout));
		CHECK(sout.x == expected);
		CHECK(sout.y == expected + 100);
	}
	CHECK(!TestStruct_extract(&spq, &sout));
	CHECK(TestStruct_is_empty(&spq));
	TestStruct_free(&spq);

	return true;
}

static void run_tests(void) {
	RUN_TEST(test_init);
	RUN_TEST(test_is_full);
	RUN_TEST(test_is_empty);
	RUN_TEST(test_reserve);
	RUN_TEST(test_insert);
	RUN_TEST(test_extract);
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
