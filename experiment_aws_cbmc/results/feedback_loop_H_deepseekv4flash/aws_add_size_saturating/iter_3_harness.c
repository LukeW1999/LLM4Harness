#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_add_size_saturating_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    size_t r = aws_add_size_saturating(a, b);

    // Check that the result is either the sum or SIZE_MAX
    assert(r == a + b || r == SIZE_MAX);

    // If overflow occurred, result must be SIZE_MAX
    if (a > SIZE_MAX - b) {
        assert(r == SIZE_MAX);
    } else {
        assert(r == a + b);
    }
}
