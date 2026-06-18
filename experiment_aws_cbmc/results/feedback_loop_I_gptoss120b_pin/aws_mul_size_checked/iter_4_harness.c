#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_mul_size_checked_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t result;

    bool overflow = aws_mul_size_checked(a, b, &result);

    bool expected_overflow = (a != 0 && b > SIZE_MAX / a);
    assert(overflow == expected_overflow);

    if (!overflow) {
        assert(result == a * b);
    }
}
