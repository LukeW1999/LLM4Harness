#include <aws/common/math.h>
#include <stdint.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_add_size_checked_harness() {
    /* Non-deterministic inputs */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    /* Local variable to hold the output, and a pointer to it */
    size_t r_val;
    size_t *r = &r_val;

    /* Assumption: r pointer must be valid (non-null) and writable */
    __CPROVER_assume(r != NULL);

    /* Save the old value of *r before the call (for failure check) */
    size_t old_r = *r;

    /* Call the function */
    int result = aws_add_size_checked(a, b, r);

    /* Postconditions based on Doxygen and implementation */
    if (b > 0 && a > (SIZE_MAX - b)) {
        /* Overflow expected: function must return error and not modify *r */
        assert(result == AWS_OP_ERR);
        assert(*r == old_r);
    } else {
        /* No overflow: function must succeed and set *r = a + b */
        assert(result == AWS_OP_SUCCESS);
        assert(*r == a + b);
    }

    /* Scalar parameters are unchanged (trivially true by value semantics) */
    /* No validity invariants to check for primitive types */
}
