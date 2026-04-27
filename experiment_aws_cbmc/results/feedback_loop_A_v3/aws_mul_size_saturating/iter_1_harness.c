#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_mul_size_checked_harness() {
    /* 1. Declare data structure(s) on stack */
    size_t a;
    size_t b;
    size_t r;

    /* 2. Bound the structure (limits CBMC state space) */
    a = nondet_size_t();
    b = nondet_size_t();

    /* 3. Allocate pointer members */
    /* No pointer members to allocate for size_t */

    /* 4. Assume validity precondition */
    /* No specific validity preconditions for size_t */

    /* 5. Save old state (for checking immutability) */
    /* No need to save state for size_t inputs */

    /* 6. Assume function-specific preconditions (from Requires: annotations) */
    /* No specific preconditions mentioned in the Requires section */

    /* 7. Call function under test */
    int result = aws_mul_size_checked(a, b, &r);

    /* 8. Assert postconditions (from Ensures: annotations) */
    if (result == AWS_OP_SUCCESS) {
        assert(r == a * b);
    } else if (result == AWS_OP_ERR) {
        assert(a * b > SIZE_MAX);
    }
    /* Invariants: No specific invariants mentioned for this function */
}
