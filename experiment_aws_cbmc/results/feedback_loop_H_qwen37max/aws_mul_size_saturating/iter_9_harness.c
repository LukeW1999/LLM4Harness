#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>

void aws_mul_size_saturating_harness() {
    size_t a;
    size_t b;

    size_t result = aws_mul_size_saturating(a, b);

    if (a == 0 || b == 0) {
        assert(result == 0);
    } else if (a > SIZE_MAX / b) {
        assert(result == SIZE_MAX);
    } else {
        assert(result == a * b);
    }
}
