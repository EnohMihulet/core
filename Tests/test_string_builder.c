#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "test.h"

#define CORE_STRING_BUILDER_IMPLEMENTATION
#include "../Strings/string_builder.h"

STRING_BUILDER_DEFINE(test_string)

static int tests_run = 0;
static int tests_failed = 0;

static bool test_init_clear_and_free(void) {
	test_string_StringBuilder sb;

	CHECK(test_string_init(&sb));
	CHECK(sb.items != NULL);
	CHECK(sb.size == 0);
	CHECK(sb.capacity > 0);
	CHECK(test_string_is_empty(&sb));
	CHECK(strcmp(test_string_cstr(&sb), "") == 0);

	CHECK(test_string_append_cstr(&sb, "content"));
	test_string_clear(&sb);
	CHECK(test_string_is_empty(&sb));
	CHECK(strcmp(test_string_cstr(&sb), "") == 0);
	CHECK(sb.capacity > 0);

	test_string_free(&sb);
	CHECK(sb.items == NULL);
	CHECK(sb.size == 0);
	CHECK(sb.capacity == 0);
	return true;
}

static bool test_append_and_growth(void) {
	test_string_StringBuilder sb;

	CHECK(test_string_init(&sb));
	CHECK(test_string_append_char(&sb, 'A'));
	CHECK(test_string_append_cstr(&sb, " short "));
	CHECK(test_string_append_slice(&sb, "slice ignored", 5));
	CHECK(strcmp(test_string_cstr(&sb), "A short slice") == 0);
	CHECK(sb.size == strlen("A short slice"));

	for (int i = 0; i < 100; ++i) {
		CHECK(test_string_append_char(&sb, (char)('a' + i % 26)));
	}
	CHECK(sb.size == strlen("A short slice") + 100);
	CHECK(test_string_cstr(&sb)[sb.size] == '\0');

	const size_t old_capacity = sb.capacity;
	CHECK(test_string_reserve(&sb, old_capacity + 100));
	CHECK(sb.capacity >= old_capacity + 100);
	CHECK(strncmp(test_string_cstr(&sb), "A short slice", strlen("A short slice")) == 0);

	test_string_free(&sb);
	return true;
}

static bool test_insert_erase_and_format(void) {
	test_string_StringBuilder sb;

	CHECK(test_string_init(&sb));
	CHECK(test_string_append_cstr(&sb, "world"));
	CHECK(test_string_insert(&sb, 0, "hello "));
	CHECK(test_string_insert(&sb, sb.size, "!"));
	CHECK(strcmp(test_string_cstr(&sb), "hello world!") == 0);
	CHECK(!test_string_insert(&sb, sb.size + 1, "invalid"));

	CHECK(test_string_erase(&sb, 5, 1));
	CHECK(strcmp(test_string_cstr(&sb), "helloworld!") == 0);
	CHECK(test_string_erase(&sb, 10, 1));
	CHECK(strcmp(test_string_cstr(&sb), "helloworld") == 0);
	CHECK(!test_string_erase(&sb, sb.size + 1, 1));

	test_string_clear(&sb);
	CHECK(test_string_append_format(&sb, "%s %d %.1f", "value", 42, 2.5));
	CHECK(strcmp(test_string_cstr(&sb), "value 42 2.5") == 0);

	test_string_free(&sb);
	return true;
}

static bool test_take_transfers_ownership(void) {
	test_string_StringBuilder sb;

	CHECK(test_string_init(&sb));
	CHECK(test_string_append_cstr(&sb, "owned string"));
	char* owned = test_string_take(&sb);
	CHECK(owned != NULL);
	CHECK(strcmp(owned, "owned string") == 0);
	CHECK(sb.items == NULL);
	CHECK(sb.size == 0);
	CHECK(sb.capacity == 0);
	free(owned);

	CHECK(test_string_init(&sb));
	CHECK(test_string_is_empty(&sb));
	test_string_free(&sb);
	return true;
}

int main(void) {
	RUN_TEST(test_init_clear_and_free);
	RUN_TEST(test_append_and_growth);
	RUN_TEST(test_insert_erase_and_format);
	RUN_TEST(test_take_transfers_ownership);

	if (tests_failed != 0) {
		fprintf(stderr, "\n%d/%d tests failed\n", tests_failed, tests_run);
		return 1;
	}
	fprintf(stdout, "\nAll %d tests passed\n", tests_run);
	return 0;
}
