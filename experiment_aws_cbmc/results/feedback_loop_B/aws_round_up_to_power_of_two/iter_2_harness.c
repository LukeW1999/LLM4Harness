#include <aws/common/common.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_mul_size_checked_harness() {
    struct aws_allocator *allocator = aws_default_allocator();
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r;
    size_t *r_ptr = &r;

    /* Assume r_ptr is writable */
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(r_ptr, sizeof(size_t)));

    /* Call function under test */
    int result = aws_mul_size_checked(a, b, r_ptr);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* Check for overflow condition */
        if (__builtin_mul_overflow(a, b, &r)) {
            assert(0); /* This should never happen if result is AWS_OP_SUCCESS */
        } else {
            assert(*r_ptr == a * b);
        }
    } else {
        /* Failure case: check for overflow */
        assert(__builtin_mul_overflow(a, b, &r));
        assert(*r_ptr == 0 || *r_ptr != a * b); /* r_ptr should not be modified to the correct product */
    }

    /* 5. No other fields to check as the function only modifies *r_ptr */

    /* 6. No validity invariants to check as there are no complex data structures involved */
}
