#include <aws/common/math.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_mul_size_saturating_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    size_t result = aws_mul_size_saturating(a, b);

    if (a == 0 || b == 0) {
        assert(result == 0);
    } else {
        if (a <= SIZE_MAX / b) {
            assert(result == a * b);
        } else {
            assert(result == SIZE_MAX);
        }
    }
}

void aws_mul_size_saturating_harness(void) {
    aws_mul_size_saturating_harness();
    return 0;
}
