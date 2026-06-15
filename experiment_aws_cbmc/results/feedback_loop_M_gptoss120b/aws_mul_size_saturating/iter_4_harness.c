#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_mul_size_saturating_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t result;

    int ret = aws_mul_size_saturating(a, b, &result);

    if (a == 0 || b == 0) {
        __CPROVER_assert(ret == 0, "ret should be zero when an operand is zero");
        __CPROVER_assert(result == 0, "result should be zero when an operand is zero");
    } else if (a > SIZE_MAX / b) {
        __CPROVER_assert(ret != 0, "ret should be non‑zero on overflow");
        __CPROVER_assert(result == SIZE_MAX, "result should be SIZE_MAX on overflow");
    } else {
        __CPROVER_assert(ret == 0, "ret should be zero when no overflow");
        __CPROVER_assert(result == a * b, "result should equal the product when no overflow");
    }
}
