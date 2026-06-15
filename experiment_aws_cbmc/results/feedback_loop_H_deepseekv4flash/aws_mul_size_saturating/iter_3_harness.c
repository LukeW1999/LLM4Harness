#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>

void aws_mul_size_saturating_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t result = aws_mul_size_saturating(a, b);

    assert(result == (b == 0 ? 0 : (a > SIZE_MAX / b ? SIZE_MAX : a * b)));
}
