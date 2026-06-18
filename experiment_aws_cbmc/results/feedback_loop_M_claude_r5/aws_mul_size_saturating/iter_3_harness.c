#include <aws/common/math.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>

void harness(void) {
    size_t a;
    size_t b;

    size_t result = aws_mul_size_saturating(a, b);

    /* If no overflow, result == a * b */
    if (a != 0 && result / a != b) {
        /* overflow occurred, result should be SIZE_MAX */
        assert(result == SIZE_MAX);
    } else if (a == 0) {
        assert(result == 0);
    } else {
        /* no overflow */
        assert(result == a * b);
    }

    /* result is always <= SIZE_MAX (trivially true for size_t) */
    assert(result <= SIZE_MAX);
}
