#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>

void aws_mul_size_saturating_harness(void) {
    size_t a = (size_t)nondet_uint64_t();
    size_t b = (size_t)nondet_uint64_t();

    __CPROVER_assume(a <= SIZE_MAX);
    __CPROVER_assume(b <= SIZE_MAX);

    size_t result;
    size_t *result_ptr = &result;

    size_t old_a = a;
    size_t old_b = b;

    bool overflow = aws_mul_size_saturating(a, b, result_ptr);

    bool expected_overflow = (a != 0 && b > SIZE_MAX / a);

    assert(overflow == expected_overflow);
    if (!expected_overflow) {
        assert(*result_ptr == a * b);
    } else {
        assert(*result_ptr == SIZE_MAX);
    }

    assert(a == old_a);
    assert(b == old_b);
}
