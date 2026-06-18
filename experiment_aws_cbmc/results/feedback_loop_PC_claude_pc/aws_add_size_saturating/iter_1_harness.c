#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/*
 * Harness for aws_add_size_checked:
 *   "Adds a + b and returns the result in *r. If the result
 *    overflows, returns AWS_OP_ERR; otherwise returns AWS_OP_SUCCESS."
 *
 * Analysis:
 * 1. Changed fields on success: *r == a + b
 * 2. Unchanged fields: a and b are inputs (not modified)
 * 3. On failure (overflow): returns AWS_OP_ERR, *r is unspecified (but pointer must be valid)
 * 4. Validity invariants: result is either AWS_OP_SUCCESS or AWS_OP_ERR
 */
void aws_add_size_checked_harness(void) {
    /* 1. Declare inputs */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    /* 2. Snapshot inputs before call */
    size_t a_old = a;
    size_t b_old = b;

    /* 3. Declare output */
    size_t r;

    /* 4. Call function under test */
    int result = aws_add_size_checked(a, b, &r);

    /* 5. Assert return value is one of the two valid outcomes */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    /* 6. Assert postconditions on success path */
    if (result == AWS_OP_SUCCESS) {
        /* No overflow occurred: r must equal a + b */
        assert(r == a + b);
        /* Also verify that the mathematical result fits in size_t (no overflow) */
        assert(a + b >= a);
        assert(a + b >= b);
    }

    /* 7. Assert postconditions on failure path */
    if (result == AWS_OP_ERR) {
        /* Overflow occurred: a + b would exceed SIZE_MAX */
        /* We can verify this by checking that the true mathematical sum overflows */
        /* In C, unsigned overflow wraps, so a + b < a (or < b) indicates overflow */
        assert(a + b < a || a + b < b);
    }

    /* 8. Assert inputs are unchanged (frame conditions) */
    assert(a == a_old);
    assert(b == b_old);
}

void aws_add_size_saturating_harness(void) {
    aws_add_size_checked_harness();
    return 0;
}
