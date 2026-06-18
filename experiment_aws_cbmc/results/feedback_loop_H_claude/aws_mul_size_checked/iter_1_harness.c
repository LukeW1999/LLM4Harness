#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_add_u32_checked_harness() {
    uint32_t a = nondet_uint32_t();
    uint32_t b = nondet_uint32_t();
    uint32_t r;

    int result = aws_add_u32_checked(a, b, &r);

    if (result == AWS_OP_SUCCESS) {
        /* On success: r == a + b, and no overflow occurred */
        assert(result == 0);
        assert(r == a + b);
        /* No overflow means a + b <= UINT32_MAX */
        assert((uint64_t)a + (uint64_t)b <= UINT32_MAX);
    } else {
        /* On failure: overflow occurred, result is AWS_OP_ERR */
        assert(result == -1);
        /* Overflow means a + b > UINT32_MAX */
        assert((uint64_t)a + (uint64_t)b > UINT32_MAX);
    }
}

void aws_add_u64_checked_harness() {
    uint64_t a = nondet_uint64_t();
    uint64_t b = nondet_uint64_t();
    uint64_t r;

    int result = aws_add_u64_checked(a, b, &r);

    if (result == AWS_OP_SUCCESS) {
        assert(result == 0);
        assert(r == a + b);
        /* No overflow: b <= UINT64_MAX - a */
        assert(b <= UINT64_MAX - a);
    } else {
        assert(result == -1);
        /* Overflow: b > UINT64_MAX - a */
        assert(b > UINT64_MAX - a);
    }
}

void aws_add_u32_saturating_harness() {
    uint32_t a = nondet_uint32_t();
    uint32_t b = nondet_uint32_t();

    uint32_t result = aws_add_u32_saturating(a, b);

    if ((uint64_t)a + (uint64_t)b > UINT32_MAX) {
        /* Overflow: returns 2^32 - 1 */
        assert(result == UINT32_MAX);
    } else {
        assert(result == a + b);
    }
}

void aws_add_u64_saturating_harness() {
    uint64_t a = nondet_uint64_t();
    uint64_t b = nondet_uint64_t();

    uint64_t result = aws_add_u64_saturating(a, b);

    if (b > UINT64_MAX - a) {
        /* Overflow: returns 2^64 - 1 */
        assert(result == UINT64_MAX);
    } else {
        assert(result == a + b);
    }
}

void aws_mul_u32_checked_harness() {
    uint32_t a = nondet_uint32_t();
    uint32_t b = nondet_uint32_t();
    uint32_t r;

    int result = aws_mul_u32_checked(a, b, &r);

    if (result == AWS_OP_SUCCESS) {
        assert(result == 0);
        assert(r == a * b);
        assert((uint64_t)a * (uint64_t)b <= UINT32_MAX);
    } else {
        assert(result == -1);
        assert((uint64_t)a * (uint64_t)b > UINT32_MAX);
    }
}

void aws_mul_u64_checked_harness() {
    uint64_t a = nondet_uint64_t();
    uint64_t b = nondet_uint64_t();
    uint64_t r;

    int result = aws_mul_u64_checked(a, b, &r);

    if (result == AWS_OP_SUCCESS) {
        assert(result == 0);
        assert(r == a * b);
        /* No overflow */
        if (a != 0) {
            assert(b <= UINT64_MAX / a);
        }
    } else {
        assert(result == -1);
        /* Overflow occurred */
        if (a != 0) {
            assert(b > UINT64_MAX / a);
        }
    }
}

void aws_mul_u32_saturating_harness() {
    uint32_t a = nondet_uint32_t();
    uint32_t b = nondet_uint32_t();

    uint32_t result = aws_mul_u32_saturating(a, b);

    if ((uint64_t)a * (uint64_t)b > UINT32_MAX) {
        assert(result == UINT32_MAX);
    } else {
        assert(result == a * b);
    }
}

void aws_mul_u64_saturating_harness() {
    uint64_t a = nondet_uint64_t();
    uint64_t b = nondet_uint64_t();

    uint64_t result = aws_mul_u64_saturating(a, b);

    bool overflow = (a != 0) && (b > UINT64_MAX / a);

    if (overflow) {
        assert(result == UINT64_MAX);
    } else {
        assert(result == a * b);
    }
}

void aws_sub_u32_saturating_harness() {
    uint32_t a = nondet_uint32_t();
    uint32_t b = nondet_uint32_t();

    uint32_t result = aws_sub_u32_saturating(a, b);

    if (b > a) {
        /* Underflow: returns 0 */
        assert(result == 0);
    } else {
        assert(result == a - b);
    }
}

void aws_sub_u64_saturating_harness() {
    uint64_t a = nondet_uint64_t();
    uint64_t b = nondet_uint64_t();

    uint64_t result = aws_sub_u64_saturating(a, b);

    if (b > a) {
        /* Underflow: returns 0 */
        assert(result == 0);
    } else {
        assert(result == a - b);
    }
}

void aws_sub_u32_checked_harness() {
    uint32_t a = nondet_uint32_t();
    uint32_t b = nondet_uint32_t();
    uint32_t r;

    int result = aws_sub_u32_checked(a, b, &r);

    if (result == AWS_OP_SUCCESS) {
        assert(result == 0);
        assert(r == a - b);
        assert(b <= a);
    } else {
        assert(result == -1);
        assert(b > a);
    }
}

void aws_sub_u64_checked_harness() {
    uint64_t a = nondet_uint64_t();
    uint64_t b = nondet_uint64_t();
    uint64_t r;

    int result = aws_sub_u64_checked(a, b, &r);

    if (result == AWS_OP_SUCCESS) {
        assert(result == 0);
        assert(r == a - b);
        assert(b <= a);
    } else {
        assert(result == -1);
        assert(b > a);
    }
}

void aws_add_size_saturating_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    size_t result = aws_add_size_saturating(a, b);

    if (b > SIZE_MAX - a) {
        assert(result == SIZE_MAX);
    } else {
        assert(result == a + b);
    }
}

void aws_add_size_checked_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r;

    int result = aws_add_size_checked(a, b, &r);

    if (result == AWS_OP_SUCCESS) {
        assert(result == 0);
        assert(r == a + b);
        assert(b <= SIZE_MAX - a);
    } else {
        assert(result == -1);
        assert(b > SIZE_MAX - a);
    }
}

void aws_mul_size_saturating_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    size_t result = aws_mul_size_saturating(a, b);

    bool overflow = (a != 0) && (b > SIZE_MAX / a);

    if (overflow) {
        assert(result == SIZE_MAX);
    } else {
        assert(result == a * b);
    }
}

void aws_mul_size_checked_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r;

    int result = aws_mul_size_checked(a, b, &r);

    bool overflow = (a != 0) && (b > SIZE_MAX / a);

    if (result == AWS_OP_SUCCESS) {
        assert(result == 0);
        assert(r == a * b);
        assert(!overflow);
    } else {
        assert(result == -1);
        assert(overflow);
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

    int result = aws_sub_size_checked(a, b, &r);

    if (result == AWS_OP_SUCCESS) {
        assert(result == 0);
        assert(r == a - b);
        assert(b <= a);
    } else {
        assert(result == -1);
        assert(b > a);
    }
}

/* Main harness entry point - call one of the above */
void harness() {
    aws_add_u64_checked_harness();
    aws_add_u32_checked_harness();
    aws_add_u64_saturating_harness();
    aws_add_u32_saturating_harness();
    aws_mul_u64_checked_harness();
    aws_mul_u32_checked_harness();
    aws_mul_u64_saturating_harness();
    aws_mul_u32_saturating_harness();
    aws_sub_u64_saturating_harness();
    aws_sub_u32_saturating_harness();
    aws_sub_u64_checked_harness();
    aws_sub_u32_checked_harness();
    aws_add_size_saturating_harness();
    aws_add_size_checked_harness();
    aws_mul_size_saturating_harness();
    aws_mul_size_checked_harness();
    aws_sub_size_saturating_harness();
    aws_sub_size_checked_harness();
}
