#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>

void aws_mul_size_saturating_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    size_t result_storage = nondet_size_t();
    size_t *result = &result_storage;

    size_t old_a = a;
    size_t old_b = b;
    size_t old_result = result_storage;

    bool overflow = aws_mul_size_saturating(a, b, result);

    bool expected_overflow = (a != 0 && b > SIZE_MAX / a);

    assert(overflow == expected_overflow);
    if (!expected_overflow) {
        assert(*result == a * b);
    } else {
        assert(*result == old_result);
    }

    assert(a == old_a);
    assert(b == old_b);
}
