#include <aws/common/common.h>
#include <aws/common/math.h>
#include <assert.h>
#include <limits.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_mul_size_saturating_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t old_a = a;
    size_t old_b = b;

    size_t result = aws_mul_size_saturating(a, b);

    // Step 1: Success path
    if (result != SIZE_MAX) {
        assert(result == a * b);
    }

    // Step 2: Failure path
    if (result == SIZE_MAX) {
        assert(a > 0 && b > SIZE_MAX / a);
    }

    // Step 3: Frame conditions
    assert(a == old_a);
    assert(b == old_b);
}
