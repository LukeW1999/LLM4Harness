#include <aws/common/common.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <stddef.h>
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
    /* No pointer members to allocate for this function */

    /* 4. Assume validity precondition */
    /* No specific validity precondition needed for primitive types */

    /* 5. Save old state (for checking immutability) */
    /* No state to save for this function */

    /* 6. Assume function-specific preconditions (from Requires: annotations) */
    /* No specific preconditions mentioned in the Requires section for this function */

    /* 7. Call function under test */
    int result = aws_mul_size_checked(a, b, &r);

    /* 8. Assert postconditions (from Ensures: annotations) */
    if (result == AWS_OP_SUCCESS) {
        /* On success, the result should be a * b */
        assert(r == a * b);
    } else if (result == AWS_OP_ERR) {
        /* On failure, the result should be unchanged */
        /* No specific postcondition for r on failure, but we know the operation errored */
    }
    /* Invariant: the function should always return either AWS_OP_SUCCESS or AWS_OP_ERR */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);
}
