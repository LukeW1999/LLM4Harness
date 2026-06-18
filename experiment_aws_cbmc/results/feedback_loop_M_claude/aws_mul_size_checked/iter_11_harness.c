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

    /* Bound inputs to avoid state space explosion */
    __CPROVER_assume(lhs <= 0xFFFF);
    __CPROVER_assume(rhs <= 0xFFFF);

    int ret = aws_mul_size_checked(lhs, rhs, &result);

    assert(ret == AWS_OP_SUCCESS || ret == AWS_OP_ERR);

    if (ret == AWS_OP_SUCCESS) {
        assert(result == lhs * rhs);
    }
}
