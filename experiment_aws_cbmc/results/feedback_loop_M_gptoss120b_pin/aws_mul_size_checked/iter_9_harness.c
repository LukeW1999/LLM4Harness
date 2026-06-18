#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>
#include <limits.h>

void aws_mul_size_checked_harness(void) {
    size_t a = __CPROVER_nondet_size_t();
    size_t b = __CPROVER_nondet_size_t();
    __CPROVER_assume(a <= 1000);
    __CPROVER_assume(b <= 1000);
    size_t result = __CPROVER_nondet_size_t();
    size_t old_result = result;

    bool overflow = aws_mul_size_checked(a, b, &result);

    bool expected_overflow = (a != 0 && b > SIZE_MAX / a);
    assert(overflow == expected_overflow);

    if (!expected_overflow) {
        assert(result == a * b);
    } else {
        assert(result == old_result);
    }
}
