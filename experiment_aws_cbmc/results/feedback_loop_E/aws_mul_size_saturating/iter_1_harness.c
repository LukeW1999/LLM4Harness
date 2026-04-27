#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_mul_size_saturating_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r = aws_mul_size_saturating(a, b);

    // Check if multiplication overflows
    bool overflow = (a > 0) && (b > SIZE_MAX / a);

    if (overflow) {
        assert(r == SIZE_MAX);
    } else {
        assert(r == a * b);
    }

    // No fields to check for unchanged values as this function returns a value and does not modify any structure
    // Validity invariant: No structure to validate as this function does not take any structure as input/output
}
