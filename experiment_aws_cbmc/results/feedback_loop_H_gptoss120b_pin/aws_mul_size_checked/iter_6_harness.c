#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdbool.h>
#include <assert.h>

void aws_mul_size_checked_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    size_t result = nondet_size_t();
    size_t old_result = result;

    bool overflow = aws_mul_size_checked(a, b, &result);

    bool actual_overflow = (a != 0 && b > SIZE_MAX / a);
    assert(overflow == actual_overflow);

    if (!overflow) {
        assert(result == a * b);
    } else {
        assert(result == old_result);
    }
}
