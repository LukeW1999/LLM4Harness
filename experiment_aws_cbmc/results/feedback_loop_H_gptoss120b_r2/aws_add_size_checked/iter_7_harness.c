#include <aws/common/math.h>
#include <aws/common/error.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_add_size_checked_harness(void) {
    size_t a = (size_t)nondet_uint64_t();
    size_t b = (size_t)nondet_uint64_t();

    size_t old_r = (size_t)nondet_uint64_t();
    size_t r = old_r;

    int result = aws_add_size_checked(a, b, &r);

    __CPROVER_assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR,
                     "result must be success or error");

    if (result == AWS_OP_SUCCESS) {
        __CPROVER_assert(r == a + b, "result should be sum on success");
    } else {
        __CPROVER_assert(r == old_r, "result should be unchanged on overflow");
    }
}
