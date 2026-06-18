#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_add_size_checked_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r;

    int result = aws_add_size_checked(a, b, &r);

    if (result == AWS_OP_SUCCESS) {
        /* On success: r == a + b, and no overflow occurred */
        assert(r == a + b);
        /* No overflow means a + b <= SIZE_MAX */
        assert(a <= SIZE_MAX - b);
    } else {
        /* On failure: overflow occurred, result is AWS_OP_ERR */
        assert(result == -1);
        /* Overflow means a + b > SIZE_MAX */
        assert(a > SIZE_MAX - b);
    }
}

void aws_add_u64_checked_harness(void) {
    uint64_t a = nondet_uint64_t();
    uint64_t b = nondet_uint64_t();
    uint64_t r;

    int result = aws_add_u64_checked(a, b, &r);

    if (result == AWS_OP_SUCCESS) {
        assert(r == a + b);
        assert(a <= UINT64_MAX - b);
    } else {
        assert(result == -1);
        assert(a > UINT64_MAX - b);
    }
}

void aws_add_u32_checked_harness(void) {
    uint32_t a = nondet_uint32_t();
    uint32_t b = nondet_uint32_t();
    uint32_t r;

    int result = aws_add_u32_checked(a, b, &r);

    if (result == AWS_OP_SUCCESS) {
        assert(r == a + b);
        assert(a <= UINT32_MAX - b);
    } else {
        assert(result == -1);
        assert(a > UINT32_MAX - b);
    }
}

void aws_mul_u64_checked_harness(void) {
    uint64_t a = nondet_uint64_t();
    uint64_t b = nondet_uint64_t();
    uint64_t r;

    int result = aws_mul_u64_checked(a, b, &r);

    if (result == AWS_OP_SUCCESS) {
        assert(r == a * b);
        /* No overflow: if a != 0, then b <= UINT64_MAX / a */
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

void aws_mul_u32_checked_harness(void) {
    uint32_t a = nondet_uint32_t();
    uint32_t b = nondet_uint32_t();
    uint32_t r;

    int result = aws_mul_u32_checked(a, b, &r);

    if (result == AWS_OP_SUCCESS) {
        assert(r == a * b);
        if (a != 0) {
            assert(b <= UINT32_MAX / a);
        }
    } else {
        assert(result == -1);
        if (a != 0) {
            assert(b > UINT32_MAX / a);
        }
    }
}

void aws_mul_size_checked_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r;

    int result = aws_mul_size_checked(a, b, &r);

    if (result == AWS_OP_SUCCESS) {
        assert(r == a * b);
        if (a != 0) {
            assert(b <= SIZE_MAX / a);
        }
    } else {
        assert(result == -1);
        if (a != 0) {
            assert(b > SIZE_MAX / a);
        }
    }
}

void aws_sub_u64_checked_harness(void) {
    uint64_t a = nondet_uint64_t();
    uint64_t b = nondet_uint64_t();
    uint64_t r;

    int result = aws_sub_u64_checked(a, b, &r);

    if (result == AWS_OP_SUCCESS) {
        assert(r == a - b);
        assert(a >= b);
    } else {
        assert(result == -1);
        assert(a < b);
    }
}

void aws_sub_u32_checked_harness(void) {
    uint32_t a = nondet_uint32_t();
    uint32_t b = nondet_uint32_t();
    uint32_t r;

    int result = aws_sub_u32_checked(a, b, &r);

    if (result == AWS_OP_SUCCESS) {
        assert(r == a - b);
        assert(a >= b);
    } else {
        assert(result == -1);
        assert(a < b);
    }
}

void aws_sub_size_checked_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r;

    int result = aws_sub_size_checked(a, b, &r);

    if (result == AWS_OP_SUCCESS) {
        assert(r == a - b);
        assert(a >= b);
    } else {
        assert(result == -1);
        assert(a < b);
    }
}

/* Main harness entry point - test aws_add_size_checked as the primary target */
void harness(void) {
    aws_add_size_checked_harness();
    aws_add_u64_checked_harness();
    aws_add_u32_checked_harness();
    aws_mul_u64_checked_harness();
    aws_mul_u32_checked_harness();
    aws_mul_size_checked_harness();
    aws_sub_u64_checked_harness();
    aws_sub_u32_checked_harness();
    aws_sub_size_checked_harness();
}
