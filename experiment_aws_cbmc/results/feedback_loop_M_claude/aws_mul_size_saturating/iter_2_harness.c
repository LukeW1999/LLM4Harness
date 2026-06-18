#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void harness(void) {
    size_t a;
    size_t b;

    size_t result = aws_mul_size_saturating(a, b);

    /* If no overflow, result == a * b */
    /* If overflow, result == SIZE_MAX */
    assert(result == SIZE_MAX || result == a * b);

    /* Verify saturation: result is always >= 0 (trivially true for size_t) */
    /* If a and b are both non-zero and result < a, then overflow occurred and result == SIZE_MAX */
    if (a != 0 && b != 0) {
        if (result != SIZE_MAX) {
            assert(result == a * b);
            assert(result / a == b);
        }
    }

    if (a == 0 || b == 0) {
        assert(result == 0);
    }
}
