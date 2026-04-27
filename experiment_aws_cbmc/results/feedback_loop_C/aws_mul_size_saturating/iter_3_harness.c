#include <aws/common/common.h>
#include <aws/common/math.h>
#include <assert.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_mul_size_saturating_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t result;

    // Capture old values of inputs (though they should remain unchanged)
    size_t old_a = a;
    size_t old_b = b;

    result = aws_mul_size_saturating(a, b);

    // Check that inputs are unchanged
    assert(a == old_a);
    assert(b == old_b);

    // Check the result based on overflow condition
    if (__builtin_mul_overflow(a, b, &result)) {
        assert(result == SIZE_MAX);
    } else {
        assert(result == a * b);
    }
}
