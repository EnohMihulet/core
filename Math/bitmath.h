#pragma once

#include <stdbool.h>
#include <stdint.h>

/*
	NOTE:
	#define CORE_BITMATH_IMPLEMENTATION
	before you include this file in only one C or C++ file

	NOTE:
	Integer bit manipulation helpers.

BITMATH_DEFINE(PREFIX) generates:

bool     PREFIX##_is_power_of_two_u64(uint64_t value);
uint64_t PREFIX##_next_power_of_two_u64(uint64_t value);
uint64_t PREFIX##_prev_power_of_two_u64(uint64_t value);
uint64_t PREFIX##_align_up_u64(uint64_t value, uint64_t alignment);
uint64_t PREFIX##_align_down_u64(uint64_t value, uint64_t alignment);
uint8_t  PREFIX##_bit_mask_u8(uint32_t bit);
uint8_t  PREFIX##_bit_set_u8(uint8_t value, uint32_t bit);
uint8_t  PREFIX##_bit_clear_u8(uint8_t value, uint32_t bit);
uint8_t  PREFIX##_bit_toggle_u8(uint8_t value, uint32_t bit);
uint8_t  PREFIX##_bit_write_u8(uint8_t value, uint32_t bit, bool bit_value);
bool     PREFIX##_bit_read_u8(uint8_t value, uint32_t bit);
uint16_t PREFIX##_bit_mask_u16(uint32_t bit);
uint16_t PREFIX##_bit_set_u16(uint16_t value, uint32_t bit);
uint16_t PREFIX##_bit_clear_u16(uint16_t value, uint32_t bit);
uint16_t PREFIX##_bit_toggle_u16(uint16_t value, uint32_t bit);
uint16_t PREFIX##_bit_write_u16(uint16_t value, uint32_t bit, bool bit_value);
bool     PREFIX##_bit_read_u16(uint16_t value, uint32_t bit);
uint32_t PREFIX##_bit_mask_u32(uint32_t bit);
uint32_t PREFIX##_bit_set_u32(uint32_t value, uint32_t bit);
uint32_t PREFIX##_bit_clear_u32(uint32_t value, uint32_t bit);
uint32_t PREFIX##_bit_toggle_u32(uint32_t value, uint32_t bit);
uint32_t PREFIX##_bit_write_u32(uint32_t value, uint32_t bit, bool bit_value);
bool     PREFIX##_bit_read_u32(uint32_t value, uint32_t bit);
uint64_t PREFIX##_bit_mask_u64(uint32_t bit);
uint64_t PREFIX##_bit_set_u64(uint64_t value, uint32_t bit);
uint64_t PREFIX##_bit_clear_u64(uint64_t value, uint32_t bit);
uint64_t PREFIX##_bit_toggle_u64(uint64_t value, uint32_t bit);
uint64_t PREFIX##_bit_write_u64(uint64_t value, uint32_t bit, bool bit_value);
bool     PREFIX##_bit_read_u64(uint64_t value, uint32_t bit);
uint64_t PREFIX##_field_mask_u64(uint32_t shift, uint32_t width);
uint64_t PREFIX##_field_read_u64(uint64_t value, uint32_t shift, uint32_t width);
uint64_t PREFIX##_field_write_u64(uint64_t value, uint32_t shift, uint32_t width, uint64_t field_value);
uint32_t PREFIX##_popcount_u64(uint64_t value);
uint32_t PREFIX##_leading_zero_count_u64(uint64_t value);
uint32_t PREFIX##_trailing_zero_count_u64(uint64_t value);
uint64_t PREFIX##_reverse_bits_u64(uint64_t value);
*/

#ifdef CORE_BITMATH_IMPLEMENTATION
	#define CORE_BITMATH_IMPL(...) __VA_ARGS__
#else
	#define CORE_BITMATH_IMPL(...)
#endif

#define BITMATH_DEFINE(PREFIX)                                                                                           \
	uint32_t PREFIX##_popcount_u64(uint64_t value) {                                                                 \
		uint32_t count = 0;                                                                                      \
		while (value != 0) {                                                                                     \
			value &= value - 1;                                                                              \
			count++;                                                                                         \
		}                                                                                                        \
		return count;                                                                                            \
	}                                                                                                                \
	uint32_t PREFIX##_leading_zero_count_u64(uint64_t value) {                                                       \
		return value == 0 ? 64u : (uint32_t)__builtin_clzll(value);                                              \
	}                                                                                                                \
                                                                                                                         \
	uint32_t PREFIX##_trailing_zero_count_u64(uint64_t value) {                                                      \
		return value == 0 ? 64u : (uint32_t)__builtin_ctzll(value);                                              \
	}                                                                                                                \
                                                                                                                         \
	bool PREFIX##_is_power_of_two_u64(uint64_t value) {                                                              \
		return value != 0 && (value & (value - 1)) == 0;                                                         \
	}                                                                                                                \
                                                                                                                         \
	uint64_t PREFIX##_next_power_of_two_u64(uint64_t value) {                                                        \
		if (value <= 1) return 1;                                                                                \
		value--;                                                                                                 \
		value |= value >> 1;                                                                                     \
		value |= value >> 2;                                                                                     \
		value |= value >> 4;                                                                                     \
		value |= value >> 8;                                                                                     \
		value |= value >> 16;                                                                                    \
		value |= value >> 32;                                                                                    \
		return value + 1;                                                                                        \
	}                                                                                                                \
                                                                                                                         \
	uint64_t PREFIX##_prev_power_of_two_u64(uint64_t value) {                                                        \
		if (value == 0) return 0;                                                                                \
		return UINT64_C(1) << (63u - PREFIX##_leading_zero_count_u64(value));                                    \
	}                                                                                                                \
                                                                                                                         \
	uint64_t PREFIX##_align_up_u64(uint64_t value, uint64_t alignment) {                                             \
		return (value + alignment - 1) & ~(alignment - 1);                                                       \
	}                                                                                                                \
                                                                                                                         \
	uint64_t PREFIX##_align_down_u64(uint64_t value, uint64_t alignment) {                                           \
		return value & ~(alignment - 1);                                                                         \
	}                                                                                                                \
                                                                                                                         \
	uint8_t PREFIX##_bit_mask_u8(uint32_t bit) {                                                                     \
		return (uint8_t)(UINT8_C(1) << bit);                                                                     \
	}                                                                                                                \
                                                                                                                         \
	uint8_t PREFIX##_bit_set_u8(uint8_t value, uint32_t bit) {                                                       \
		return (uint8_t)(value | PREFIX##_bit_mask_u8(bit));                                                     \
	}                                                                                                                \
                                                                                                                         \
	uint8_t PREFIX##_bit_clear_u8(uint8_t value, uint32_t bit) {                                                     \
		return (uint8_t)(value & (uint8_t)~PREFIX##_bit_mask_u8(bit));                                           \
	}                                                                                                                \
                                                                                                                         \
	uint8_t PREFIX##_bit_toggle_u8(uint8_t value, uint32_t bit) {                                                    \
		return (uint8_t)(value ^ PREFIX##_bit_mask_u8(bit));                                                     \
	}                                                                                                                \
                                                                                                                         \
	uint8_t PREFIX##_bit_write_u8(uint8_t value, uint32_t bit, bool bit_value) {                                     \
		return bit_value ? PREFIX##_bit_set_u8(value, bit) : PREFIX##_bit_clear_u8(value, bit);                  \
	}                                                                                                                \
                                                                                                                         \
	bool PREFIX##_bit_read_u8(uint8_t value, uint32_t bit) {                                                         \
		return (value & PREFIX##_bit_mask_u8(bit)) != 0;                                                         \
	}                                                                                                                \
                                                                                                                         \
	uint16_t PREFIX##_bit_mask_u16(uint32_t bit) {                                                                   \
		return (uint16_t)(UINT16_C(1) << bit);                                                                   \
	}                                                                                                                \
                                                                                                                         \
	uint16_t PREFIX##_bit_set_u16(uint16_t value, uint32_t bit) {                                                    \
		return (uint16_t)(value | PREFIX##_bit_mask_u16(bit));                                                   \
	}                                                                                                                \
                                                                                                                         \
	uint16_t PREFIX##_bit_clear_u16(uint16_t value, uint32_t bit) {                                                  \
		return (uint16_t)(value & (uint16_t)~PREFIX##_bit_mask_u16(bit));                                        \
	}                                                                                                                \
                                                                                                                         \
	uint16_t PREFIX##_bit_toggle_u16(uint16_t value, uint32_t bit) {                                                 \
		return (uint16_t)(value ^ PREFIX##_bit_mask_u16(bit));                                                   \
	}                                                                                                                \
                                                                                                                         \
	uint16_t PREFIX##_bit_write_u16(uint16_t value, uint32_t bit, bool bit_value) {                                  \
		return bit_value ? PREFIX##_bit_set_u16(value, bit) : PREFIX##_bit_clear_u16(value, bit);                \
	}                                                                                                                \
                                                                                                                         \
	bool PREFIX##_bit_read_u16(uint16_t value, uint32_t bit) {                                                       \
		return (value & PREFIX##_bit_mask_u16(bit)) != 0;                                                        \
	}                                                                                                                \
                                                                                                                         \
	uint32_t PREFIX##_bit_mask_u32(uint32_t bit) {                                                                   \
		return UINT32_C(1) << bit;                                                                               \
	}                                                                                                                \
                                                                                                                         \
	uint32_t PREFIX##_bit_set_u32(uint32_t value, uint32_t bit) {                                                    \
		return value | PREFIX##_bit_mask_u32(bit);                                                               \
	}                                                                                                                \
                                                                                                                         \
	uint32_t PREFIX##_bit_clear_u32(uint32_t value, uint32_t bit) {                                                  \
		return value & ~PREFIX##_bit_mask_u32(bit);                                                              \
	}                                                                                                                \
                                                                                                                         \
	uint32_t PREFIX##_bit_toggle_u32(uint32_t value, uint32_t bit) {                                                 \
		return value ^ PREFIX##_bit_mask_u32(bit);                                                               \
	}                                                                                                                \
                                                                                                                         \
	uint32_t PREFIX##_bit_write_u32(uint32_t value, uint32_t bit, bool bit_value) {                                  \
		return bit_value ? PREFIX##_bit_set_u32(value, bit) : PREFIX##_bit_clear_u32(value, bit);                \
	}                                                                                                                \
                                                                                                                         \
	bool PREFIX##_bit_read_u32(uint32_t value, uint32_t bit) {                                                       \
		return (value & PREFIX##_bit_mask_u32(bit)) != 0;                                                        \
	}                                                                                                                \
                                                                                                                         \
	uint64_t PREFIX##_bit_mask_u64(uint32_t bit) {                                                                   \
		return UINT64_C(1) << bit;                                                                               \
	}                                                                                                                \
                                                                                                                         \
	uint64_t PREFIX##_bit_set_u64(uint64_t value, uint32_t bit) {                                                    \
		return value | PREFIX##_bit_mask_u64(bit);                                                               \
	}                                                                                                                \
                                                                                                                         \
	uint64_t PREFIX##_bit_clear_u64(uint64_t value, uint32_t bit) {                                                  \
		return value & ~PREFIX##_bit_mask_u64(bit);                                                              \
	}                                                                                                                \
                                                                                                                         \
	uint64_t PREFIX##_bit_toggle_u64(uint64_t value, uint32_t bit) {                                                 \
		return value ^ PREFIX##_bit_mask_u64(bit);                                                               \
	}                                                                                                                \
                                                                                                                         \
	uint64_t PREFIX##_bit_write_u64(uint64_t value, uint32_t bit, bool bit_value) {                                  \
		return bit_value ? PREFIX##_bit_set_u64(value, bit) : PREFIX##_bit_clear_u64(value, bit);                \
	}                                                                                                                \
                                                                                                                         \
	bool PREFIX##_bit_read_u64(uint64_t value, uint32_t bit) {                                                       \
		return (value & PREFIX##_bit_mask_u64(bit)) != 0;                                                        \
	}                                                                                                                \
                                                                                                                         \
	uint64_t PREFIX##_field_mask_u64(uint32_t shift, uint32_t width) {                                               \
		uint64_t mask = width >= 64 ? UINT64_MAX : ((UINT64_C(1) << width) - 1);                                 \
		return mask << shift;                                                                                    \
	}                                                                                                                \
                                                                                                                         \
	uint64_t PREFIX##_field_read_u64(uint64_t value, uint32_t shift, uint32_t width) {                               \
		return (value & PREFIX##_field_mask_u64(shift, width)) >> shift;                                         \
	}                                                                                                                \
                                                                                                                         \
	uint64_t PREFIX##_field_write_u64(uint64_t value, uint32_t shift, uint32_t width, uint64_t field_value) {        \
		uint64_t mask = PREFIX##_field_mask_u64(shift, width);                                                   \
		return (value & ~mask) | ((field_value << shift) & mask);                                                \
	}                                                                                                                \
                                                                                                                         \
	uint64_t PREFIX##_reverse_bits_u64(uint64_t value) {                                                             \
		value = ((value >> 1) & UINT64_C(0x5555555555555555)) | ((value & UINT64_C(0x5555555555555555)) << 1);   \
		value = ((value >> 2) & UINT64_C(0x3333333333333333)) | ((value & UINT64_C(0x3333333333333333)) << 2);   \
		value = ((value >> 4) & UINT64_C(0x0F0F0F0F0F0F0F0F)) | ((value & UINT64_C(0x0F0F0F0F0F0F0F0F)) << 4);   \
		value = ((value >> 8) & UINT64_C(0x00FF00FF00FF00FF)) | ((value & UINT64_C(0x00FF00FF00FF00FF)) << 8);   \
		value = ((value >> 16) & UINT64_C(0x0000FFFF0000FFFF)) | ((value & UINT64_C(0x0000FFFF0000FFFF)) << 16); \
		return (value >> 32) | (value << 32);                                                                    \
	}                                                                                                                \
	CORE_BITMATH_IMPL()
