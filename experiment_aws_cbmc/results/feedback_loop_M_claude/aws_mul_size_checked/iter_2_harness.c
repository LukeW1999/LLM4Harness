#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_mul_size_checked_harness(void) {
    size_t lhs;
    size_t rhs;
    size_t result;

    /* Non-deterministic inputs */
    __CPROVER_assume(lhs <= SIZE_MAX);
    __CPROVER_assume(rhs <= SIZE_MAX);

    int ret = aws_mul_size_checked(lhs, rhs, &result);

    /* Return value must be success or error */
    assert(ret == AWS_OP_SUCCESS || ret == AWS_OP_ERR);

    if (ret == AWS_OP_SUCCESS) {
        /* On success, result must equal lhs * rhs */
        assert(result == lhs * rhs);
        /* Verify no overflow occurred */
        if (lhs != 0) {
            assert(result / lhs == rhs);
        }
        if (rhs != 0) {
            assert(result / rhs == lhs);
        }
    } else {
        /* On error, overflow must have occurred */
        assert(ret == AWS_OP_ERR);
        /* If both are non-zero and overflow occurred, lhs * rhs > SIZE_MAX */
        if (lhs != 0 && rhs != 0) {
            assert(lhs > SIZE_MAX / rhs);
        }
    }
}
