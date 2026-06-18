#include <aws/common/math.h>
#include <stddef.h>
#include <limits.h>
#include <stdbool.h>
#include <assert.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_mul_size_saturating_harness(void) {
    size_t a = (size_t)nondet_uint64_t();
    size_t b = (size_t)nondet_uint64_t();

    const size_t a_old = a;
    const size_t b_old = b;

    size_t result = aws_mul_size_saturating(a, b);

    bool overflow = (a_old != 0 && b_old > SIZE_MAX / a_old);
    if (!overflow) {
        assert(result == a_old * b_old);
    } else {
        assert(result == SIZE_MAX);
    }
    assert(result <= SIZE_MAX);
}
