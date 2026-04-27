#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>

void aws_mul_size_checked_harness() {
    /* 1. Declare data structure(s) on stack */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r;
    size_t *r_ptr = &r;

    /* 2. Bound the structure (limits CBMC state space) */
    /* No structures to bound in this case */

    /* 3. Allocate pointer members */
    /* No pointer members to allocate in this case */

    /* 4. Assume validity precondition */
    /* No additional validity preconditions needed for size_t types */

    /* 5. Save old state (for checking immutability) */
    /* No state to save as r is an output parameter */

    /* 6. Assume function-specific preconditions */
    /* No additional function-specific preconditions needed */

    /* 7. Call function under test */
    int result = aws_mul_size_checked(a, b, r_ptr);

    /* 8. Assert postconditions (both branches) */
    if (result == AWS_OP_SUCCESS) {
        assert(r == a * b);
    } else {
        assert(result == AWS_OP_ERR);
        assert(r == SIZE_MAX);
    }
}
