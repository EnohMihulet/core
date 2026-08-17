#include <stdbool.h>
#include <stdio.h>

#include "test.h"

#define CORE_BITSET_IMPLEMENTATION
#include "../DataStructures/bitset.h"

BITSET_DEFINE(test_bits, 130)

static int tests_run = 0;
static int tests_failed = 0;

static bool test_init_and_free(void) {
	test_bits_BitSet bits;

	CHECK(test_bits_init(&bits));
	CHECK(bits.bit_count == 130);
	CHECK(bits.word_count == 3);
	CHECK(bits.words != NULL);
	CHECK(test_bits_count(&bits) == 0);
	CHECK(!test_bits_any(&bits));
	CHECK(test_bits_none(&bits));
	CHECK(!test_bits_all(&bits));

	test_bits_free(&bits);
	CHECK(bits.bit_count == 0);
	CHECK(bits.word_count == 0);
	CHECK(bits.words == NULL);
	return true;
}

static bool test_individual_bits_and_bounds(void) {
	test_bits_BitSet bits;
	bool value = true;

	CHECK(test_bits_init(&bits));
	CHECK(test_bits_set(&bits, 0));
	CHECK(test_bits_set(&bits, 63));
	CHECK(test_bits_set(&bits, 64));
	CHECK(test_bits_set(&bits, 129));
	CHECK(test_bits_count(&bits) == 4);

	CHECK(test_bits_get(&bits, 0, &value) && value);
	CHECK(test_bits_get(&bits, 63, &value) && value);
	CHECK(test_bits_get(&bits, 64, &value) && value);
	CHECK(test_bits_get(&bits, 129, &value) && value);
	CHECK(test_bits_get(&bits, 1, &value) && !value);

	CHECK(test_bits_unset(&bits, 63));
	CHECK(test_bits_toggle(&bits, 64));
	CHECK(test_bits_assign(&bits, 1, true));
	CHECK(test_bits_assign(&bits, 129, false));
	CHECK(test_bits_count(&bits) == 2);
	CHECK(test_bits_get(&bits, 1, &value) && value);

	value = true;
	CHECK(!test_bits_get(&bits, 130, &value));
	CHECK(value);
	CHECK(!test_bits_set(&bits, 130));
	CHECK(!test_bits_unset(&bits, 130));
	CHECK(!test_bits_toggle(&bits, 130));
	CHECK(!test_bits_assign(&bits, 130, true));

	test_bits_free(&bits);
	return true;
}

static bool test_bulk_operations(void) {
	test_bits_BitSet bits;
	bool value = false;

	CHECK(test_bits_init(&bits));
	CHECK(test_bits_set_all(&bits));
	CHECK(test_bits_all(&bits));
	CHECK(test_bits_any(&bits));
	CHECK(!test_bits_none(&bits));
	CHECK(test_bits_count(&bits) == bits.bit_count);
	CHECK(test_bits_get(&bits, 129, &value) && value);

	CHECK(test_bits_unset_all(&bits));
	CHECK(test_bits_none(&bits));
	CHECK(test_bits_count(&bits) == 0);

	CHECK(test_bits_set(&bits, 5));
	CHECK(test_bits_set(&bits, 100));
	test_bits_clear(&bits);
	CHECK(test_bits_none(&bits));
	CHECK(test_bits_count(&bits) == 0);

	test_bits_free(&bits);
	return true;
}

static bool test_resize_preserves_bits(void) {
	test_bits_BitSet bits;
	bool value = true;

	CHECK(test_bits_init(&bits));
	CHECK(test_bits_set(&bits, 5));
	CHECK(test_bits_set(&bits, 129));

	CHECK(test_bits_resize(&bits, 193));
	CHECK(bits.bit_count == 193);
	CHECK(bits.word_count == 4);
	CHECK(test_bits_get(&bits, 5, &value) && value);
	CHECK(test_bits_get(&bits, 129, &value) && value);
	CHECK(test_bits_get(&bits, 130, &value) && !value);
	CHECK(test_bits_get(&bits, 192, &value) && !value);

	CHECK(test_bits_set(&bits, 192));
	CHECK(test_bits_resize(&bits, 65));
	CHECK(bits.bit_count == 65);
	CHECK(bits.word_count == 2);
	CHECK(test_bits_get(&bits, 5, &value) && value);
	CHECK(!test_bits_get(&bits, 65, &value));
	CHECK(test_bits_count(&bits) == 1);

	test_bits_free(&bits);
	return true;
}

int main(void) {
	RUN_TEST(test_init_and_free);
	RUN_TEST(test_individual_bits_and_bounds);
	RUN_TEST(test_bulk_operations);
	RUN_TEST(test_resize_preserves_bits);

	if (tests_failed != 0) {
		fprintf(stderr, "\n%d/%d tests failed\n", tests_failed, tests_run);
		return 1;
	}
	fprintf(stdout, "\nAll %d tests passed\n", tests_run);
	return 0;
}
