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

    int ret = aws_mul_size_checked(lhs, rhs, &result);

    /* Return value must be success or error */
    assert(ret == AWS_OP_SUCCESS || ret == AWS_OP_ERR);

    if (ret == AWS_OP_SUCCESS) {
        /* On success, result must equal lhs * rhs */
        assert(result == lhs * rhs);
    } else {
        /* On error, overflow must have occurred */
        assert(ret == AWS_OP_ERR);
    }
}
