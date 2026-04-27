#include <aws/common/common.h>
#include <aws/common/math.h>
#include <stdint.h>
#include <stdlib.h>

#ifndef MAX_BUFFER_SIZE
#    define MAX_BUFFER_SIZE 64
#endif

void aws_mul_size_saturating_harness(void) {
    size_t a;
    size_t b;

    /* Non-deterministic inputs - no constraints needed */
    size_t result = aws_mul_size_saturating(a, b);

    /* Postconditions */
    /* If multiplication would overflow SIZE_MAX, result should be SIZE_MAX */
    /* If multiplication does not overflow, result should equal a * b */

    /* Check that result is never larger than SIZE_MAX (trivially true for size_t) */
    assert(result <= SIZE_MAX);

    /* Check saturation: if a > 0 and b > 0 and result < a, then result must be SIZE_MAX */
    if (a > 0 && b > 0) {
        if (result < a) {
            assert(result == SIZE_MAX);
        }
        if (result < b) {
            assert(result == SIZE_MAX);
        }
    }

    /* Check: if a == 0 or b == 0, result must be 0 */
    if (a == 0 || b == 0) {
        assert(result == 0);
    }

    /* Check: if result != SIZE_MAX, then result == a * b (no overflow occurred) */
    /* We verify this by checking: result * 1 == result and a * b / a == b when no overflow */
    if (a > 0 && b > 0 && result != SIZE_MAX) {
        /* result should equal a * b exactly */
        assert(result / a == b);
        assert(result / b == a);
    }

    /* Check: if a == 1, result should be b (or SIZE_MAX if b == SIZE_MAX, but b == SIZE_MAX is fine) */
    if (a == 1) {
        assert(result == b);
    }

    /* Check: if b == 1, result should be a */
    if (b == 1) {
        assert(result == a);
    }
}
