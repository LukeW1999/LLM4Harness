#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

extern size_t nondet_size_t(void);

void aws_mul_size_checked_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    __CPROVER_assume(a <= 1000);
    __CPROVER_assume(b <= 1000);

    size_t result = nondet_size_t();
    size_t old_result = result;

    bool overflow = aws_mul_size_checked(a, b, &result);

    bool expected_overflow = (a != 0 && b != 0 && SIZE_MAX / a < b);
    __CPROVER_assert(overflow == expected_overflow, "overflow matches expected");

    if (!overflow) {
        __CPROVER_assert(result == a * b, "result correct when no overflow");
    } else {
        __CPROVER_assert(result == old_result, "result unchanged on overflow");
    }
}
