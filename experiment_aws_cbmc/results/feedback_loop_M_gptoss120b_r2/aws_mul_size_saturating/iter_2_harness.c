#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_mul_size_saturating_harness(void) {
    /* Non‑deterministic inputs */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    /* Allocate storage for the result */
    size_t result_storage;
    size_t *result = &result_storage;

    /* Save old state */
    size_t old_a = a;
    size_t old_b = b;
    size_t old_result = result_storage;

    /* Call the function under test */
    bool overflow = aws_mul_size_saturating(a, b, result);

    /* Compute the expected overflow condition */
    bool expected_overflow = (a != 0 && b > SIZE_MAX / a);

    /* Post‑conditions */
    assert(overflow == expected_overflow);
    if (!expected_overflow) {
        assert(*result == a * b);
    } else {
        /* On overflow the result is unspecified; it must remain unchanged */
        assert(*result == old_result);
    }
    /* Inputs must remain unchanged */
    assert(a == old_a);
    assert(b == old_b);
}
