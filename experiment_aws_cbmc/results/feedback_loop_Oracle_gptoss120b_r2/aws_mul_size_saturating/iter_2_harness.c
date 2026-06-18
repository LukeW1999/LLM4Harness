#include <aws/common/math.h>
#include <assert.h>
#include <stddef.h>
#include <limits.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_mul_size_saturating_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    size_t result = aws_mul_size_saturating(a, b);

    bool overflow = (b != 0 && a > SIZE_MAX / b);

    if (!overflow) {
        assert(result == a * b);
    } else {
        assert(result == SIZE_MAX);
    }
}
