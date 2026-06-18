#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_mul_size_saturating_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    size_t result = aws_mul_size_saturating(a, b);

    size_t product = a * b;
    if (a != 0 && product / a != b) {
        assert(result == SIZE_MAX);
    } else {
        assert(result == product);
    }

    if (result == SIZE_MAX) {
        size_t product2 = a * b;
        assert(a != 0 && product2 / a != b);
    }

    if (result != SIZE_MAX) {
        assert(result == a * b);
    }
}
