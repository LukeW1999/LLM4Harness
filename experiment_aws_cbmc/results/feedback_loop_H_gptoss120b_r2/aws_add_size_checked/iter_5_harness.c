#include <proof_helpers/make_common_data_structures.h>

void aws_add_u64_checked_harness(void) {
    uint64_t a = nondet_uint64_t();
    uint64_t b = nondet_uint64_t();

    uint64_t old_r = nondet_uint64_t();
    uint64_t r = old_r;

    int result = aws_add_u64_checked(a, b, &r);

    __CPROVER_assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR,
                     "result must be success or error");

    if (result == AWS_OP_SUCCESS) {
        __CPROVER_assert(r == a + b, "result should be sum on success");
    } else {
        __CPROVER_assert(r == old_r, "result should be unchanged on overflow");
    }
}
