#ifndef AWS_COMMON_MATH_INL
#define AWS_COMMON_MATH_INL

/**
 * ESBMC-compatible math.inl override.
 *
 * Forces all u32/u64 operations to use math.fallback.inl (pure C, no CBMC or
 * GCC-builtin intrinsics). Then re-defines the size_t wrappers that delegate
 * to those fallback implementations.
 *
 * Note: aws_add_size_checked_varargs is AWS_COMMON_API (non-static) and is
 * implemented in source/math.c — not redefined here.
 */

#include <aws/common/common.h>
#include <aws/common/math.h>

/* Pull in the portable u32/u64 implementations */
#include <aws/common/math.fallback.inl>

/* ── size_t wrappers ──────────────────────────────────────────────────────── */

AWS_STATIC_IMPL size_t aws_mul_size_saturating(size_t a, size_t b) {
#if SIZE_BITS == 32
    return (size_t)aws_mul_u32_saturating(a, b);
#else
    return (size_t)aws_mul_u64_saturating(a, b);
#endif
}

AWS_STATIC_IMPL int aws_mul_size_checked(size_t a, size_t b, size_t *r) {
#if SIZE_BITS == 32
    return aws_mul_u32_checked(a, b, (uint32_t *)r);
#else
    return aws_mul_u64_checked(a, b, (uint64_t *)r);
#endif
}

AWS_STATIC_IMPL size_t aws_add_size_saturating(size_t a, size_t b) {
#if SIZE_BITS == 32
    return (size_t)aws_add_u32_saturating(a, b);
#else
    return (size_t)aws_add_u64_saturating(a, b);
#endif
}

AWS_STATIC_IMPL int aws_add_size_checked(size_t a, size_t b, size_t *r) {
#if SIZE_BITS == 32
    return aws_add_u32_checked(a, b, (uint32_t *)r);
#else
    return aws_add_u64_checked(a, b, (uint64_t *)r);
#endif
}

AWS_STATIC_IMPL size_t aws_sub_size_saturating(size_t a, size_t b) {
#if SIZE_BITS == 32
    return (size_t)aws_sub_u32_saturating(a, b);
#else
    return (size_t)aws_sub_u64_saturating(a, b);
#endif
}

AWS_STATIC_IMPL int aws_sub_size_checked(size_t a, size_t b, size_t *r) {
#if SIZE_BITS == 32
    return aws_sub_u32_checked(a, b, (uint32_t *)r);
#else
    return aws_sub_u64_checked(a, b, (uint64_t *)r);
#endif
}

AWS_STATIC_IMPL bool aws_is_power_of_two(const size_t x) {
    return x != 0 && (x & (x - 1)) == 0;
}

AWS_STATIC_IMPL int aws_round_up_to_power_of_two(size_t n, size_t *result) {
    if (n == 0) {
        *result = 1;
        return AWS_OP_SUCCESS;
    }
    if (n > SIZE_MAX_POWER_OF_TWO) {
        return aws_raise_error(AWS_ERROR_OVERFLOW_DETECTED);
    }
    size_t power = 1;
    while (power < n) {
        power <<= 1;
    }
    *result = power;
    return AWS_OP_SUCCESS;
}

AWS_STATIC_IMPL size_t aws_min_size(size_t a, size_t b) {
    return a < b ? a : b;
}

AWS_STATIC_IMPL size_t aws_max_size(size_t a, size_t b) {
    return a > b ? a : b;
}

AWS_STATIC_IMPL int32_t aws_min_i32(int32_t a, int32_t b) {
    return a < b ? a : b;
}

AWS_STATIC_IMPL int32_t aws_max_i32(int32_t a, int32_t b) {
    return a > b ? a : b;
}

AWS_STATIC_IMPL int64_t aws_min_i64(int64_t a, int64_t b) {
    return a < b ? a : b;
}

AWS_STATIC_IMPL int64_t aws_max_i64(int64_t a, int64_t b) {
    return a > b ? a : b;
}

#endif /* AWS_COMMON_MATH_INL */
