#include <aws/common/common.h>
#include <aws/common/math.h>
#include <assert.h>
#include <stddef.h>

void aws_mul_size_checked_harness() {
    /* 1. Declare and bound data structures */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r;
    size_t *r_ptr = &r;

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    size_t old_r = r;

    /* 3. Call function under test */
    int result = aws_mul_size_checked(a, b, r_ptr);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        assert(r == a * b);
    } else {
        assert(r == old_r);  // Ensure r is unchanged on failure
    }

    /* 5. Assert fields that must NOT change regardless of result */
    // No other fields to assert as this function only uses primitive types

    /* 6. Assert validity invariants always holds */
    // No complex structure to validate
}
