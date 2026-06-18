#include <proof_helpers/make_common_data_structures.h>

void aws_mul_size_checked_harness(void) {
    size_t lhs;
    size_t rhs;
    size_t result = 0;

    /* Bound the inputs to avoid state space explosion */
    __CPROVER_assume(lhs <= 0xFFFF);
    __CPROVER_assume(rhs <= 0xFFFF);

    int ret = aws_mul_size_checked(lhs, rhs, &result);

    __CPROVER_assert(ret == AWS_OP_SUCCESS || ret == AWS_OP_ERR, "ret is success or error");

    if (ret == AWS_OP_SUCCESS) {
        __CPROVER_assert(result == lhs * rhs, "result equals lhs * rhs on success");
    }
}
