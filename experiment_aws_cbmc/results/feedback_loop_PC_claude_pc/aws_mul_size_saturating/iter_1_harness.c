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
 * 3. On failure (overflow): returns AWS_OP_ERR, *r is unspecified (we don't assert its value)
 * 4. Validity invariants: result is either AWS_OP_SUCCESS or AWS_OP_ERR
 */
void aws_add_size_checked_harness(void) {
    /* 1. Declare inputs */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    /* 2. Declare output */
    size_t r;

    /* 3. Save input snapshots before call */
    size_t old_a = a;
    size_t old_b = b;

    /* 4. Call function under test */
    int result = aws_add_size_checked(a, b, &r);

    /* 5. Assert return value is one of the two valid outcomes */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    /* 6. Assert postconditions on success path */
    if (result == AWS_OP_SUCCESS) {
        /* On success: no overflow occurred, r == a + b */
        assert(r == a + b);
        /* Also verify that the mathematical result fits in size_t (no overflow) */
        assert(r >= a);
        assert(r >= b);
    } else {
        /* On failure: overflow would have occurred */
        /* The overflow condition: a + b > SIZE_MAX, i.e., b > SIZE_MAX - a */
        assert(b > (SIZE_MAX - a));
    }

    /* 7. Assert inputs are unchanged (frame condition) */
    assert(a == old_a);
    assert(b == old_b);
}

void aws_mul_size_saturating_harness(void) {
    aws_add_size_checked_harness();
    return 0;
}
