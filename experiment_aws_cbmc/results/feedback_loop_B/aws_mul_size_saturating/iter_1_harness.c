#include <aws/common/common.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>
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
        assert(r == SIZE_MAX);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(a == old_a);
    assert(b == old_b);

    /* 6. Assert validity invariant always holds */
    /* No additional validity invariants for primitive types */
}
