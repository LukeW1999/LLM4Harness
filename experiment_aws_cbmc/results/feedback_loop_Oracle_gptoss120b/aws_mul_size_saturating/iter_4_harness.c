#include <aws/common/math.h>
#include <stdbool.h>
#include <assert.h>
#include <stddef.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_mul_size_saturating_harness(void) {
    size_t a = (size_t)nondet_uint64_t();
    size_t b = (size_t)nondet_uint64_t();

    size_t result = aws_mul_size_saturating(a, b);

    bool overflow = (a != 0 && b > SIZE_MAX / a);
    size_t expected = overflow ? SIZE_MAX : a * b;

    assert(result == expected);
}
