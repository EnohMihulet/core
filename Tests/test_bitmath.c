#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "test.h"

#define CORE_BITMATH_IMPLEMENTATION
#include "../Math/bitmath.h"

BITMATH_DEFINE(test_bitmath)

static int tests_run = 0;
static int tests_failed = 0;

static bool test_power_of_two_helpers(void) {
	CHECK(!test_bitmath_is_power_of_two_u64(0));
	CHECK(test_bitmath_is_power_of_two_u64(1));
	CHECK(test_bitmath_is_power_of_two_u64(2));
	CHECK(!test_bitmath_is_power_of_two_u64(3));
	CHECK(test_bitmath_is_power_of_two_u64(UINT64_C(1) << 63));
	CHECK(!test_bitmath_is_power_of_two_u64(UINT64_MAX));

	CHECK(test_bitmath_next_power_of_two_u64(0) == 1);
	CHECK(test_bitmath_next_power_of_two_u64(1) == 1);
	CHECK(test_bitmath_next_power_of_two_u64(2) == 2);
	CHECK(test_bitmath_next_power_of_two_u64(3) == 4);
	CHECK(test_bitmath_next_power_of_two_u64(63) == 64);
	CHECK(test_bitmath_next_power_of_two_u64(64) == 64);
	CHECK(test_bitmath_next_power_of_two_u64(65) == 128);
	CHECK(test_bitmath_next_power_of_two_u64(UINT64_C(1) << 63) == (UINT64_C(1) << 63));

	CHECK(test_bitmath_prev_power_of_two_u64(0) == 0);
	CHECK(test_bitmath_prev_power_of_two_u64(1) == 1);
	CHECK(test_bitmath_prev_power_of_two_u64(2) == 2);
	CHECK(test_bitmath_prev_power_of_two_u64(3) == 2);
	CHECK(test_bitmath_prev_power_of_two_u64(63) == 32);
	CHECK(test_bitmath_prev_power_of_two_u64(64) == 64);
	CHECK(test_bitmath_prev_power_of_two_u64(65) == 64);
	CHECK(test_bitmath_prev_power_of_two_u64(UINT64_MAX) == (UINT64_C(1) << 63));
	return true;
}

static bool test_alignment_helpers(void) {
	CHECK(test_bitmath_align_up_u64(0, 8) == 0);
	CHECK(test_bitmath_align_up_u64(1, 8) == 8);
	CHECK(test_bitmath_align_up_u64(8, 8) == 8);
	CHECK(test_bitmath_align_up_u64(9, 8) == 16);
	CHECK(test_bitmath_align_up_u64(1000, 256) == 1024);

	CHECK(test_bitmath_align_down_u64(0, 8) == 0);
	CHECK(test_bitmath_align_down_u64(7, 8) == 0);
	CHECK(test_bitmath_align_down_u64(8, 8) == 8);
	CHECK(test_bitmath_align_down_u64(15, 8) == 8);
	CHECK(test_bitmath_align_down_u64(1025, 256) == 1024);
	return true;
}

static bool test_u8_bit_helpers(void) {
	CHECK(test_bitmath_bit_mask_u8(0) == UINT8_C(0x01));
	CHECK(test_bitmath_bit_mask_u8(7) == UINT8_C(0x80));
	CHECK(test_bitmath_bit_set_u8(UINT8_C(0x01), 7) == UINT8_C(0x81));
	CHECK(test_bitmath_bit_clear_u8(UINT8_MAX, 7) == UINT8_C(0x7f));
	CHECK(test_bitmath_bit_toggle_u8(UINT8_C(0x80), 7) == 0);
	CHECK(test_bitmath_bit_toggle_u8(0, 0) == UINT8_C(0x01));
	CHECK(test_bitmath_bit_write_u8(0, 7, true) == UINT8_C(0x80));
	CHECK(test_bitmath_bit_write_u8(UINT8_MAX, 7, false) == UINT8_C(0x7f));
	CHECK(test_bitmath_bit_read_u8(UINT8_C(0x80), 7));
	CHECK(!test_bitmath_bit_read_u8(UINT8_C(0x80), 6));
	return true;
}

static bool test_u16_bit_helpers(void) {
	CHECK(test_bitmath_bit_mask_u16(0) == UINT16_C(0x0001));
	CHECK(test_bitmath_bit_mask_u16(15) == UINT16_C(0x8000));
	CHECK(test_bitmath_bit_set_u16(UINT16_C(0x0001), 15) == UINT16_C(0x8001));
	CHECK(test_bitmath_bit_clear_u16(UINT16_MAX, 15) == UINT16_C(0x7fff));
	CHECK(test_bitmath_bit_toggle_u16(UINT16_C(0x8000), 15) == 0);
	CHECK(test_bitmath_bit_toggle_u16(0, 0) == UINT16_C(0x0001));
	CHECK(test_bitmath_bit_write_u16(0, 15, true) == UINT16_C(0x8000));
	CHECK(test_bitmath_bit_write_u16(UINT16_MAX, 15, false) == UINT16_C(0x7fff));
	CHECK(test_bitmath_bit_read_u16(UINT16_C(0x8000), 15));
	CHECK(!test_bitmath_bit_read_u16(UINT16_C(0x8000), 14));
	return true;
}

static bool test_u32_bit_helpers(void) {
	CHECK(test_bitmath_bit_mask_u32(0) == UINT32_C(0x00000001));
	CHECK(test_bitmath_bit_mask_u32(31) == UINT32_C(0x80000000));
	CHECK(test_bitmath_bit_set_u32(UINT32_C(0x00000001), 31) == UINT32_C(0x80000001));
	CHECK(test_bitmath_bit_clear_u32(UINT32_MAX, 31) == UINT32_C(0x7fffffff));
	CHECK(test_bitmath_bit_toggle_u32(UINT32_C(0x80000000), 31) == 0);
	CHECK(test_bitmath_bit_toggle_u32(0, 0) == UINT32_C(0x00000001));
	CHECK(test_bitmath_bit_write_u32(0, 31, true) == UINT32_C(0x80000000));
	CHECK(test_bitmath_bit_write_u32(UINT32_MAX, 31, false) == UINT32_C(0x7fffffff));
	CHECK(test_bitmath_bit_read_u32(UINT32_C(0x80000000), 31));
	CHECK(!test_bitmath_bit_read_u32(UINT32_C(0x80000000), 30));
	return true;
}

static bool test_u64_bit_helpers(void) {
	CHECK(test_bitmath_bit_mask_u64(0) == UINT64_C(0x0000000000000001));
	CHECK(test_bitmath_bit_mask_u64(63) == UINT64_C(0x8000000000000000));
	CHECK(test_bitmath_bit_set_u64(UINT64_C(0x0000000000000001), 63) == UINT64_C(0x8000000000000001));
	CHECK(test_bitmath_bit_clear_u64(UINT64_MAX, 63) == UINT64_C(0x7fffffffffffffff));
	CHECK(test_bitmath_bit_toggle_u64(UINT64_C(0x8000000000000000), 63) == 0);
	CHECK(test_bitmath_bit_toggle_u64(0, 0) == UINT64_C(0x0000000000000001));
	CHECK(test_bitmath_bit_write_u64(0, 63, true) == UINT64_C(0x8000000000000000));
	CHECK(test_bitmath_bit_write_u64(UINT64_MAX, 63, false) == UINT64_C(0x7fffffffffffffff));
	CHECK(test_bitmath_bit_read_u64(UINT64_C(0x8000000000000000), 63));
	CHECK(!test_bitmath_bit_read_u64(UINT64_C(0x8000000000000000), 62));
	return true;
}

static bool test_field_helpers(void) {
	CHECK(test_bitmath_field_mask_u64(0, 0) == 0);
	CHECK(test_bitmath_field_mask_u64(0, 4) == UINT64_C(0x000000000000000f));
	CHECK(test_bitmath_field_mask_u64(8, 8) == UINT64_C(0x000000000000ff00));
	CHECK(test_bitmath_field_mask_u64(0, 64) == UINT64_MAX);

	CHECK(test_bitmath_field_read_u64(UINT64_C(0x123456789abcdef0), 8, 8) == UINT64_C(0xde));
	CHECK(test_bitmath_field_read_u64(UINT64_MAX, 32, 16) == UINT64_C(0xffff));
	CHECK(test_bitmath_field_read_u64(UINT64_MAX, 12, 0) == 0);
	CHECK(test_bitmath_field_read_u64(UINT64_C(0x123456789abcdef0), 0, 64) ==
	      UINT64_C(0x123456789abcdef0));

	CHECK(test_bitmath_field_write_u64(UINT64_C(0xffff0000ffff0000), 16, 16, UINT64_C(0x1234)) ==
	      UINT64_C(0xffff000012340000));
	CHECK(test_bitmath_field_write_u64(0, 8, 4, UINT64_C(0xff)) == UINT64_C(0x0000000000000f00));
	CHECK(test_bitmath_field_write_u64(UINT64_C(0x1234), 4, 0, UINT64_MAX) == UINT64_C(0x1234));
	CHECK(test_bitmath_field_write_u64(UINT64_MAX, 0, 64, UINT64_C(0x0123456789abcdef)) ==
	      UINT64_C(0x0123456789abcdef));
	return true;
}

static bool test_count_helpers(void) {
	CHECK(test_bitmath_popcount_u64(0) == 0);
	CHECK(test_bitmath_popcount_u64(UINT64_MAX) == 64);
	CHECK(test_bitmath_popcount_u64(UINT64_C(0xaaaaaaaaaaaaaaaa)) == 32);
	CHECK(test_bitmath_popcount_u64(UINT64_C(1) << 63) == 1);

	CHECK(test_bitmath_leading_zero_count_u64(0) == 64);
	CHECK(test_bitmath_leading_zero_count_u64(1) == 63);
	CHECK(test_bitmath_leading_zero_count_u64(UINT64_C(1) << 63) == 0);
	CHECK(test_bitmath_leading_zero_count_u64(UINT64_C(0x100)) == 55);

	CHECK(test_bitmath_trailing_zero_count_u64(0) == 64);
	CHECK(test_bitmath_trailing_zero_count_u64(1) == 0);
	CHECK(test_bitmath_trailing_zero_count_u64(UINT64_C(1) << 63) == 63);
	CHECK(test_bitmath_trailing_zero_count_u64(UINT64_C(0x100)) == 8);
	return true;
}

static bool test_reverse_bits(void) {
	CHECK(test_bitmath_reverse_bits_u64(0) == 0);
	CHECK(test_bitmath_reverse_bits_u64(UINT64_MAX) == UINT64_MAX);
	CHECK(test_bitmath_reverse_bits_u64(1) == (UINT64_C(1) << 63));
	CHECK(test_bitmath_reverse_bits_u64(UINT64_C(1) << 63) == 1);
	CHECK(test_bitmath_reverse_bits_u64(UINT64_C(0x0123456789abcdef)) ==
	      UINT64_C(0xf7b3d591e6a2c480));
	return true;
}

int main(void) {
	RUN_TEST(test_power_of_two_helpers);
	RUN_TEST(test_alignment_helpers);
	RUN_TEST(test_u8_bit_helpers);
	RUN_TEST(test_u16_bit_helpers);
	RUN_TEST(test_u32_bit_helpers);
	RUN_TEST(test_u64_bit_helpers);
	RUN_TEST(test_field_helpers);
	RUN_TEST(test_count_helpers);
	RUN_TEST(test_reverse_bits);

	if (tests_failed != 0) {
		fprintf(stderr, "\n%d/%d tests failed\n", tests_failed, tests_run);
		return 1;
	}

	fprintf(stdout, "\nAll %d tests passed\n", tests_run);
	return 0;
}
