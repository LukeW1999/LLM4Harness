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

    __CPROVER_assume(lhs <= SIZE_MAX);
    __CPROVER_assume(rhs <= SIZE_MAX);

    int ret = aws_mul_size_checked(lhs, rhs, &result);

    assert(ret == AWS_OP_SUCCESS || ret == AWS_OP_ERR);

    if (ret == AWS_OP_SUCCESS) {
        assert(result == lhs * rhs);
    }
}
