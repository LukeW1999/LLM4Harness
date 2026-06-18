#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>

size_t nondet_size_t();

void aws_mul_size_saturating_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    size_t result = aws_mul_size_saturating(a, b);

    size_t expected;
    if (aws_mul_size_checked(a, b, &expected)) {
        assert(result == SIZE_MAX);
    } else {
        assert(result == expected);
    }
}
