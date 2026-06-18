#include <aws/common/math.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdbool.h>
#include <stddef.h>
#include <limits.h>

void aws_mul_size_checked_harness() {
    size_t a = (size_t)__CPROVER_nondet_unsigned_long();
    size_t b = (size_t)__CPROVER_nondet_unsigned_long();

    /* Limit the range to keep verification tractable */
    __CPROVER_assume(a <= 1000);
    __CPROVER_assume(b <= 1000);

    size_t result;
    bool overflow = aws_mul_size_checked(a, b, &result);

    bool expected_overflow = (a != 0 && b > SIZE_MAX / a);
    __CPROVER_assert(overflow == expected_overflow,
                     "overflow matches expected");

    if (!overflow) {
        __CPROVER_assert(result == a * b,
                         "result equals product when no overflow");
    }
}
