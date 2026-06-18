#include <aws/common/common.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_mul_size_saturating_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    size_t result = nondet_size_t();
    size_t old_result = result;
    size_t old_a = a;
    size_t old_b = b;

    int rc = aws_mul_size_saturating(a, b, &result);

    if (rc == AWS_OP_SUCCESS) {
        if (a == 0 || b == 0) {
            __CPROVER_assert(result == 0, "result should be zero when one operand is zero");
        } else {
            __CPROVER_assert(result / a == b, "result must equal a * b without overflow");
        }
    } else {
        __CPROVER_assert(result == old_result, "result must be unchanged on overflow");
    }

    __CPROVER_assert(a == old_a, "input a must remain unchanged");
    __CPROVER_assert(b == old_b, "input b must remain unchanged");
}
