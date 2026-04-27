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
    int result;

    /* 2. Bound the structure (limits CBMC state space) */
    a = nondet_size_t();
    b = nondet_size_t();

    /* 3. Allocate pointer members */
    /* No pointer members to allocate for size_t */

    /* 4. Assume validity precondition */
    /* No specific validity preconditions for size_t inputs */

    /* 5. Save old state (for checking immutability) */
    /* No state to save for size_t inputs */

    /* 6. Assume function-specific preconditions (from Requires: annotations) */
    /* No specific function-specific preconditions mentioned */

    /* 7. Call function under test */
    result = aws_mul_size_checked(a, b, &r);

    /* 8. Assert postconditions (from Ensures: annotations) */
    if (result == AWS_OP_SUCCESS) {
        assert(r == a * b); /* On success, the result is the product of a and b */
    } else if (result == AWS_OP_ERR) {
        assert(r == SIZE_MAX); /* On failure, the result is SIZE_MAX */
    }
    /* Invariant: The function should always return either AWS_OP_SUCCESS or AWS_OP_ERR */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);
}
