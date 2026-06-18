#include <aws/common/math.h>
#include <assert.h>
#include <stddef.h>
#include <limits.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_mul_size_saturating_harness(void) {
    size_t a = (size_t)nondet_uint64();
    size_t b = (size_t)nondet_uint64();

    __CPROVER_assume(a <= 10);
    __CPROVER_assume(b <= 10);

    size_t result = aws_mul_size_saturating(a, b);

    bool overflow = (b != 0 && a > SIZE_MAX / b);

    if (!overflow) {
        assert(result == a * b);
    } else {
        assert(result == SIZE_MAX);
    }
}
