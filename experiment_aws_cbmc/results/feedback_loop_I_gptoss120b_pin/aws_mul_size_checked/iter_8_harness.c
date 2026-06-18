#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdbool.h>
#include <stddef.h>
#include <limits.h>

void aws_mul_size_checked_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t result;

    bool overflow = aws_mul_size_checked(a, b, &result);

    bool expected_overflow = (a != 0 && b > SIZE_MAX / a);
    __CPROVER_assert(overflow == expected_overflow, "overflow matches expected");

    if (!overflow) {
        __CPROVER_assert(result == a * b, "result equals product when no overflow");
    }
}
