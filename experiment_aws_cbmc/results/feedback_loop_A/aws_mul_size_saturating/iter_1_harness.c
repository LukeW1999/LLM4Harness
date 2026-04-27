#include <aws/common/math.h>
#include <assert.h>

void aws_mul_size_checked_harness() {
    /* 1. Declare and bound data structures */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r;
    size_t *r_ptr = &r;

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    size_t old_a = a;
    size_t old_b = b;

    /* 3. Call function under test */
    int result = aws_mul_size_checked(a, b, r_ptr);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        assert(r == a * b);
    } else {
        assert(a == old_a);
        assert(b == old_b);
        assert(r == 0); // Assuming r is set to 0 on failure, based on typical behavior
    }

    /* 5. Assert fields that must NOT change regardless of result */
    // No other fields to assert in this case, as the function only modifies the output parameter r

    /* 6. Assert validity invariants always holds */
    // No specific validity invariants for scalar types
}
