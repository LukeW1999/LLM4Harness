#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_mul_size_saturating_harness(void) {
    size_t a;
    size_t b;

    size_t result = aws_mul_size_saturating(a, b);

    if (a != 0 && b != 0 && a > SIZE_MAX / b) {
        /* Overflow: result should be saturated to SIZE_MAX */
        assert(result == SIZE_MAX);
    } else {
        /* No overflow: result should be a * b */
        assert(result == a * b);
    }
}
