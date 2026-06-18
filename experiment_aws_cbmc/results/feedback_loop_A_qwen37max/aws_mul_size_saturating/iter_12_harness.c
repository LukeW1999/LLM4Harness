#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_mul_size_saturating_harness() {
    size_t a;
    size_t b;

    size_t result = aws_mul_size_saturating(a, b);

    if (__CPROVER_overflow_mult(a, b)) {
        assert(result == SIZE_MAX);
    } else {
        assert(result == a * b);
    }
}
