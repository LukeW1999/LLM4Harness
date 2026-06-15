#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

void aws_mul_size_saturating_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t result = aws_mul_size_saturating(a, b);

    if (a == 0 || b == 0) {
        assert(result == 0);
    } else if (a * b <= SIZE_MAX) {
        assert(result == a * b);
    } else {
        assert(result == SIZE_MAX);
    }

    assert(a == a); /* a is unchanged */
    assert(b == b); /* b is unchanged */
}
