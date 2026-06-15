#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_mul_u64_saturating_harness() {
    uint64_t a = nondet_uint64_t();
    uint64_t b = nondet_uint64_t();

    uint64_t result = aws_mul_u64_saturating(a, b);

    // If multiplication overflows, result should be UINT64_MAX
    if (a > 0 && b > UINT64_MAX / a) {
        assert(result == UINT64_MAX);
    } else {
        assert(result == a * b);
    }
}

void aws_mul_u64_checked_harness() {
    uint64_t a = nondet_uint64_t();
    uint64_t b = nondet_uint64_t();
    uint64_t r;
    uint64_t old_r = r;

    int result = aws_mul_u64_checked(a, b, &r);

    if (result == AWS_OP_SUCCESS) {
        assert(r == a * b);
    } else {
        assert(result == AWS_OP_ERR);
        assert(r == old_r); // r unchanged on overflow
    }
}

void aws_mul_u32_saturating_harness() {
    uint32_t a = nondet_uint32_t();
    uint32_t b = nondet_uint32_t();

    uint32_t result = aws_mul_u32_saturating(a, b);

    if (a > 0 && b > UINT32_MAX / a) {
        assert(result == UINT32_MAX);
    } else {
        assert(result == a * b);
    }
}

void aws_mul_u32_checked_harness() {
    uint32_t a = nondet_uint32_t();
    uint32_t b = nondet_uint32_t();
    uint32_t r;
    uint32_t old_r = r;

    int result = aws_mul_u32_checked(a, b, &r);

    if (result == AWS_OP_SUCCESS) {
        assert(r == a * b);
    } else {
        assert(result == AWS_OP_ERR);
        assert(r == old_r);
    }
}

void aws_add_u64_saturating_harness() {
    uint64_t a = nondet_uint64_t();
    uint64_t b = nondet_uint64_t();

    uint64_t result = aws_add_u64_saturating(a, b);

    if (a > UINT64_MAX - b) {
        assert(result == UINT64_MAX);
    } else {
        assert(result == a + b);
    }
}

void aws_add_u64_checked_harness() {
    uint64_t a = nondet_uint64_t();
    uint64_t b = nondet_uint64_t();
    uint64_t r;
    uint64_t old_r = r;

    int result = aws_add_u64_checked(a, b, &r);

    if (result == AWS_OP_SUCCESS) {
        assert(r == a + b);
    } else {
        assert(result == AWS_OP_ERR);
        assert(r == old_r);
    }
}

void aws_add_u32_saturating_harness() {
    uint32_t a = nondet_uint32_t();
    uint32_t b = nondet_uint32_t();

    uint32_t result = aws_add_u32_saturating(a, b);

    if (a > UINT32_MAX - b) {
        assert(result == UINT32_MAX);
    } else {
        assert(result == a + b);
    }
}

void aws_add_u32_checked_harness() {
    uint32_t a = nondet_uint32_t();
    uint32_t b = nondet_uint32_t();
    uint32_t r;
    uint32_t old_r = r;

    int result = aws_add_u32_checked(a, b, &r);

    if (result == AWS_OP_SUCCESS) {
        assert(r == a + b);
    } else {
        assert(result == AWS_OP_ERR);
        assert(r == old_r);
    }
}

void aws_sub_u64_saturating_harness() {
    uint64_t a = nondet_uint64_t();
    uint64_t b = nondet_uint64_t();

    uint64_t result = aws_sub_u64_saturating(a, b);

    if (b > a) {
        assert(result == 0);
    } else {
        assert(result == a - b);
    }
}

void aws_sub_u64_checked_harness() {
    uint64_t a = nondet_uint64_t();
    uint64_t b = nondet_uint64_t();
    uint64_t r;
    uint64_t old_r = r;

    int result = aws_sub_u64_checked(a, b, &r);

    if (result == AWS_OP_SUCCESS) {
        assert(r == a - b);
    } else {
        assert(result == AWS_OP_ERR);
        assert(r == old_r);
    }
}

void aws_sub_u32_saturating_harness() {
    uint32_t a = nondet_uint32_t();
    uint32_t b = nondet_uint32_t();

    uint32_t result = aws_sub_u32_saturating(a, b);

    if (b > a) {
        assert(result == 0);
    } else {
        assert(result == a - b);
    }
}

void aws_sub_u32_checked_harness() {
    uint32_t a = nondet_uint32_t();
    uint32_t b = nondet_uint32_t();
    uint32_t r;
    uint32_t old_r = r;

    int result = aws_sub_u32_checked(a, b, &r);

    if (result == AWS_OP_SUCCESS) {
        assert(r == a - b);
    } else {
        assert(result == AWS_OP_ERR);
        assert(r == old_r);
    }
}

void aws_mul_size_saturating_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    size_t result = aws_mul_size_saturating(a, b);

    if (a > 0 && b > SIZE_MAX / a) {
        assert(result == SIZE_MAX);
    } else {
        assert(result == a * b);
    }
}

void aws_mul_size_checked_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r;
    size_t old_r = r;

    int result = aws_mul_size_checked(a, b, &r);

    if (result == AWS_OP_SUCCESS) {
        assert(r == a * b);
    } else {
        assert(result == AWS_OP_ERR);
        assert(r == old_r);
    }
}

void aws_add_size_saturating_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    size_t result = aws_add_size_saturating(a, b);

    if (a > SIZE_MAX - b) {
        assert(result == SIZE_MAX);
    } else {
        assert(result == a + b);
    }
}

void aws_add_size_checked_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r;
    size_t old_r = r;

    int result = aws_add_size_checked(a, b, &r);

    if (result == AWS_OP_SUCCESS) {
        assert(r == a + b);
    } else {
        assert(result == AWS_OP_ERR);
        assert(r == old_r);
    }
}

void aws_sub_size_saturating_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    size_t result = aws_sub_size_saturating(a, b);

    if (b > a) {
        assert(result == 0);
    } else {
        assert(result == a - b);
    }
}

void aws_sub_size_checked_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r;
    size_t old_r = r;

    int result = aws_sub_size_checked(a, b, &r);

    if (result == AWS_OP_SUCCESS) {
        assert(r == a - b);
    } else {
        assert(result == AWS_OP_ERR);
        assert(r == old_r);
    }
}

void aws_is_power_of_two_harness() {
    size_t x = nondet_size_t();

    bool result = aws_is_power_of_two(x);

    // A number is a power of two if it is non-zero and has exactly one bit set
    if (x == 0) {
        assert(result == false);
    } else {
        assert(result == ((x & (x - 1)) == 0));
    }
}

void aws_round_up_to_power_of_two_harness() {
    size_t n = nondet_size_t();
    size_t result;
    size_t old_result = result;

    int ret = aws_round_up_to_power_of_two(n, &result);

    if (ret == AWS_OP_SUCCESS) {
        // result must be a power of two
        assert(result > 0);
        assert((result & (result - 1)) == 0);
        // result must be >= n
        assert(result >= n);
        // result must be the smallest such power of two
        if (n > 0) {
            assert(result / 2 < n);
        }
    } else {
        assert(ret == AWS_OP_ERR);
        assert(result == old_result);
    }
}

void aws_clz_u32_harness() {
    uint32_t n = nondet_uint32_t();

    size_t result = aws_clz_u32(n);

    if (n == 0) {
        assert(result == 32);
    } else {
        // result should be the number of leading zeros
        assert(result < 32);
        // Verify by shifting: (n << result) should have MSB set
        assert((n << result) & 0x80000000);
        // And (n << (result - 1)) should not have MSB set (if result > 0)
        if (result > 0) {
            assert(!((n << (result - 1)) & 0x80000000));
        }
    }
}

void aws_clz_i32_harness() {
    int32_t n = nondet_int32_t();

    size_t result = aws_clz_i32(n);

    if (n == 0) {
        assert(result == 32);
    } else {
        assert(result < 32);
        assert(((uint32_t)n << result) & 0x80000000);
        if (result > 0) {
            assert(!(((uint32_t)n << (result - 1)) & 0x80000000));
        }
    }
}

void aws_clz_u64_harness() {
    uint64_t n = nondet_uint64_t();

    size_t result = aws_clz_u64(n);

    if (n == 0) {
        assert(result == 64);
    } else {
        assert(result < 64);
        assert((n << result) & 0x8000000000000000ULL);
        if (result > 0) {
            assert(!((n << (result - 1)) & 0x8000000000000000ULL));
        }
    }
}

void aws_clz_i64_harness() {
    int64_t n = nondet_int64_t();

    size_t result = aws_clz_i64(n);

    if (n == 0) {
        assert(result == 64);
    } else {
        assert(result < 64);
        assert(((uint64_t)n << result) & 0x8000000000000000ULL);
        if (result > 0) {
            assert(!(((uint64_t)n << (result - 1)) & 0x8000000000000000ULL));
        }
    }
}

void aws_clz_size_harness() {
    size_t n = nondet_size_t();

    size_t result = aws_clz_size(n);

    if (n == 0) {
        assert(result == SIZE_BITS);
    } else {
        assert(result < SIZE_BITS);
        assert((n << result) & SIZE_MAX_POWER_OF_TWO);
        if (result > 0) {
            assert(!((n << (result - 1)) & SIZE_MAX_POWER_OF_TWO));
        }
    }
}

void aws_ctz_u32_harness() {
    uint32_t n = nondet_uint32_t();

    size_t result = aws_ctz_u32(n);

    if (n == 0) {
        assert(result == 32);
    } else {
        assert(result < 32);
        assert((n >> result) & 1);
        if (result > 0) {
            assert(!((n >> (result - 1)) & 1));
        }
    }
}

void aws_ctz_i32_harness() {
    int32_t n = nondet_int32_t();

    size_t result = aws_ctz_i32(n);

    if (n == 0) {
        assert(result == 32);
    } else {
        assert(result < 32);
        assert(((uint32_t)n >> result) & 1);
        if (result > 0) {
            assert(!(((uint32_t)n >> (result - 1)) & 1));
        }
    }
}

void aws_ctz_u64_harness() {
    uint64_t n = nondet_uint64_t();

    size_t result = aws_ctz_u64(n);

    if (n == 0) {
        assert(result == 64);
    } else {
        assert(result < 64);
        assert((n >> result) & 1);
        if (result > 0) {
            assert(!((n >> (result - 1)) & 1));
        }
    }
}

void aws_ctz_i64_harness() {
    int64_t n = nondet_int64_t();

    size_t result = aws_ctz_i64(n);

    if (n == 0) {
        assert(result == 64);
    } else {
        assert(result < 64);
        assert(((uint64_t)n >> result) & 1);
        if (result > 0) {
            assert(!(((uint64_t)n >> (result - 1)) & 1));
        }
    }
}

void aws_ctz_size_harness() {
    size_t n = nondet_size_t();

    size_t result = aws_ctz_size(n);

    if (n == 0) {
        assert(result == SIZE_BITS);
    } else {
        assert(result < SIZE_BITS);
        assert((n >> result) & 1);
        if (result > 0) {
            assert(!((n >> (result - 1)) & 1));
        }
    }
}

void aws_min_u8_harness() {
    uint8_t a = nondet_uint8_t();
    uint8_t b = nondet_uint8_t();

    uint8_t result = aws_min_u8(a, b);

    assert(result == (a < b ? a : b));
}

void aws_max_u8_harness() {
    uint8_t a = nondet_uint8_t();
    uint8_t b = nondet_uint8_t();

    uint8_t result = aws_max_u8(a, b);

    assert(result == (a > b ? a : b));
}

void aws_min_i8_harness() {
    int8_t a = nondet_int8_t();
    int8_t b = nondet_int8_t();

    int8_t result = aws_min_i8(a, b);

    assert(result == (a < b ? a : b));
}

void aws_max_i8_harness() {
    int8_t a = nondet_int8_t();
    int8_t b = nondet_int8_t();

    int8_t result = aws_max_i8(a, b);

    assert(result == (a > b ? a : b));
}

void aws_min_u16_harness() {
    uint16_t a = nondet_uint16_t();
    uint16_t b = nondet_uint16_t();

    uint16_t result = aws_min_u16(a, b);

    assert(result == (a < b ? a : b));
}

void aws_max_u16_harness() {
    uint16_t a = nondet_uint16_t();
    uint16_t b = nondet_uint16_t();

    uint16_t result = aws_max_u16(a, b);

    assert(result == (a > b ? a : b));
}

void aws_min_i16_harness() {
    int16_t a = nondet_int16_t();
    int16_t b = nondet_int16_t();

    int16_t result = aws_min_i16(a, b);

    assert(result == (a < b ? a : b));
}

void aws_max_i16_harness() {
    int16_t a = nondet_int16_t();
    int16_t b = nondet_int16_t();

    int16_t result = aws_max_i16(a, b);

    assert(result == (a > b ? a : b));
}

void aws_min_u32_harness() {
    uint32_t a = nondet_uint32_t();
    uint32_t b = nondet_uint32_t();

    uint32_t result = aws_min_u32(a, b);

    assert(result == (a < b ? a : b));
}

void aws_max_u32_harness() {
    uint32_t a = nondet_uint32_t();
    uint32_t b = nondet_uint32_t();

    uint32_t result = aws_max_u32(a, b);

    assert(result == (a > b ? a : b));
}

void aws_min_i32_harness() {
    int32_t a = nondet_int32_t();
    int32_t b = nondet_int32_t();

    int32_t result = aws_min_i32(a, b);

    assert(result == (a < b ? a : b));
}

void aws_max_i32_harness() {
    int32_t a = nondet_int32_t();
    int32_t b = nondet_int32_t();

    int32_t result = aws_max_i32(a, b);

    assert(result == (a > b ? a : b));
}

void aws_min_u64_harness() {
    uint64_t a = nondet_uint64_t();
    uint64_t b = nondet_uint64_t();

    uint64_t result = aws_min_u64(a, b);

    assert(result == (a < b ? a : b));
}

void aws_max_u64_harness() {
    uint64_t a = nondet_uint64_t();
    uint64_t b = nondet_uint64_t();

    uint64_t result = aws_max_u64(a, b);

    assert(result == (a > b ? a : b));
}

void aws_min_i64_harness() {
    int64_t a = nondet_int64_t();
    int64_t b = nondet_int64_t();

    int64_t result = aws_min_i64(a, b);

    assert(result == (a < b ? a : b));
}

void aws_max_i64_harness() {
    int64_t a = nondet_int64_t();
    int64_t b = nondet_int64_t();

    int64_t result = aws_max_i64(a, b);

    assert(result == (a > b ? a : b));
}

void aws_min_size_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    size_t result = aws_min_size(a, b);

    assert(result == (a < b ? a : b));
}

void aws_max_size_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    size_t result = aws_max_size(a, b);

    assert(result == (a > b ? a : b));
}

void aws_min_int_harness() {
    int a = nondet_int();
    int b = nondet_int();

    int result = aws_min_int(a, b);

    assert(result == (a < b ? a : b));
}

void aws_max_int_harness() {
    int a = nondet_int();
    int b = nondet_int();

    int result = aws_max_int(a, b);

    assert(result == (a > b ? a : b));
}

void aws_min_float_harness() {
    float a = nondet_float();
    float b = nondet_float();

    float result = aws_min_float(a, b);

    assert(result == (a < b ? a : b));
}

void aws_max_float_harness() {
    float a = nondet_float();
    float b = nondet_float();

    float result = aws_max_float(a, b);

    assert(result == (a > b ? a : b));
}

void aws_min_double_harness() {
    double a = nondet_double();
    double b = nondet_double();

    double result = aws_min_double(a, b);

    assert(result == (a < b ? a : b));
}

void aws_max_double_harness() {
    double a = nondet_double();
    double b = nondet_double();

    double result = aws_max_double(a, b);

    assert(result == (a > b ? a : b));
}
