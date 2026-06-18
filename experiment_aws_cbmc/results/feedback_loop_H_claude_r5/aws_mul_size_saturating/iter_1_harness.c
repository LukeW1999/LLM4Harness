#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_add_size_checked_varargs_harness(void) {
    /* aws_add_size_checked_varargs: Adds [num] arguments (expected to be of size_t),
     * and returns the result in *r.
     * If the result overflows, returns AWS_OP_ERR; otherwise returns AWS_OP_SUCCESS.
     */

    /* Use a fixed small number of arguments to keep the state space bounded.
     * We'll test with num=2 as a representative case. */
    size_t num = 2;
    size_t r;
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    int result = aws_add_size_checked_varargs(num, &r, a, b);

    /* Postconditions:
     * 1. Return value is either AWS_OP_SUCCESS (0) or AWS_OP_ERR (-1)
     * 2. On success: r == a + b (no overflow occurred)
     * 3. On failure: overflow would have occurred (a + b > SIZE_MAX)
     */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        /* No overflow: result stored in r */
        assert(r == a + b);
        /* Verify no overflow actually occurred */
        assert(a <= SIZE_MAX - b);
    } else {
        /* Overflow detected */
        assert(result == AWS_OP_ERR);
        /* Overflow means a + b > SIZE_MAX */
        assert(a > SIZE_MAX - b);
    }
}

void aws_add_u64_checked_harness(void) {
    uint64_t a = nondet_uint64_t();
    uint64_t b = nondet_uint64_t();
    uint64_t r;

    int result = aws_add_u64_checked(a, b, &r);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        assert(r == a + b);
        assert(a <= UINT64_MAX - b);
    } else {
        assert(result == AWS_OP_ERR);
        assert(a > UINT64_MAX - b);
    }
}

void aws_add_u32_checked_harness(void) {
    uint32_t a = nondet_uint32_t();
    uint32_t b = nondet_uint32_t();
    uint32_t r;

    int result = aws_add_u32_checked(a, b, &r);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        assert(r == a + b);
        assert(a <= UINT32_MAX - b);
    } else {
        assert(result == AWS_OP_ERR);
        assert(a > UINT32_MAX - b);
    }
}

void aws_add_size_saturating_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    size_t result = aws_add_size_saturating(a, b);

    if (a <= SIZE_MAX - b) {
        /* No overflow */
        assert(result == a + b);
    } else {
        /* Overflow: returns SIZE_MAX */
        assert(result == SIZE_MAX);
    }
}

void aws_mul_u64_saturating_harness(void) {
    uint64_t a = nondet_uint64_t();
    uint64_t b = nondet_uint64_t();

    uint64_t result = aws_mul_u64_saturating(a, b);

    /* If no overflow, result == a * b; otherwise result == UINT64_MAX */
    if (a == 0 || b == 0) {
        assert(result == 0);
    } else if (a <= UINT64_MAX / b) {
        assert(result == a * b);
    } else {
        assert(result == UINT64_MAX);
    }
}

void aws_mul_u64_checked_harness(void) {
    uint64_t a = nondet_uint64_t();
    uint64_t b = nondet_uint64_t();
    uint64_t r;

    int result = aws_mul_u64_checked(a, b, &r);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        assert(r == a * b);
        assert(a == 0 || b <= UINT64_MAX / a);
    } else {
        assert(result == AWS_OP_ERR);
        assert(a != 0 && b > UINT64_MAX / a);
    }
}

void aws_mul_u32_saturating_harness(void) {
    uint32_t a = nondet_uint32_t();
    uint32_t b = nondet_uint32_t();

    uint32_t result = aws_mul_u32_saturating(a, b);

    if (a == 0 || b == 0) {
        assert(result == 0);
    } else if (a <= UINT32_MAX / b) {
        assert(result == a * b);
    } else {
        assert(result == UINT32_MAX);
    }
}

void aws_mul_u32_checked_harness(void) {
    uint32_t a = nondet_uint32_t();
    uint32_t b = nondet_uint32_t();
    uint32_t r;

    int result = aws_mul_u32_checked(a, b, &r);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        assert(r == a * b);
        assert(a == 0 || b <= UINT32_MAX / a);
    } else {
        assert(result == AWS_OP_ERR);
        assert(a != 0 && b > UINT32_MAX / a);
    }
}

void aws_sub_u64_saturating_harness(void) {
    uint64_t a = nondet_uint64_t();
    uint64_t b = nondet_uint64_t();

    uint64_t result = aws_sub_u64_saturating(a, b);

    if (a >= b) {
        assert(result == a - b);
    } else {
        assert(result == 0);
    }
}

void aws_sub_u64_checked_harness(void) {
    uint64_t a = nondet_uint64_t();
    uint64_t b = nondet_uint64_t();
    uint64_t r;

    int result = aws_sub_u64_checked(a, b, &r);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        assert(r == a - b);
        assert(a >= b);
    } else {
        assert(result == AWS_OP_ERR);
        assert(a < b);
    }
}

void aws_sub_u32_saturating_harness(void) {
    uint32_t a = nondet_uint32_t();
    uint32_t b = nondet_uint32_t();

    uint32_t result = aws_sub_u32_saturating(a, b);

    if (a >= b) {
        assert(result == a - b);
    } else {
        assert(result == 0);
    }
}

void aws_sub_u32_checked_harness(void) {
    uint32_t a = nondet_uint32_t();
    uint32_t b = nondet_uint32_t();
    uint32_t r;

    int result = aws_sub_u32_checked(a, b, &r);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        assert(r == a - b);
        assert(a >= b);
    } else {
        assert(result == AWS_OP_ERR);
        assert(a < b);
    }
}

void aws_mul_size_saturating_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    size_t result = aws_mul_size_saturating(a, b);

    if (a == 0 || b == 0) {
        assert(result == 0);
    } else if (a <= SIZE_MAX / b) {
        assert(result == a * b);
    } else {
        assert(result == SIZE_MAX);
    }
}

void aws_mul_size_checked_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r;

    int result = aws_mul_size_checked(a, b, &r);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        assert(r == a * b);
        assert(a == 0 || b <= SIZE_MAX / a);
    } else {
        assert(result == AWS_OP_ERR);
        assert(a != 0 && b > SIZE_MAX / a);
    }
}

void aws_add_size_checked_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r;

    int result = aws_add_size_checked(a, b, &r);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        assert(r == a + b);
        assert(a <= SIZE_MAX - b);
    } else {
        assert(result == AWS_OP_ERR);
        assert(a > SIZE_MAX - b);
    }
}

void aws_sub_size_saturating_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    size_t result = aws_sub_size_saturating(a, b);

    if (a >= b) {
        assert(result == a - b);
    } else {
        assert(result == 0);
    }
}

void aws_sub_size_checked_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r;

    int result = aws_sub_size_checked(a, b, &r);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        assert(r == a - b);
        assert(a >= b);
    } else {
        assert(result == AWS_OP_ERR);
        assert(a < b);
    }
}

/* Main harness entry point - test the primary function */
void harness(void) {
    aws_add_size_checked_varargs_harness();
}
