#include <aws/common/math.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

void aws_add_size_saturating_harness(void) {
    size_t a;
    size_t b;

    /* No preconditions needed - function must handle all size_t inputs */

    size_t result = aws_add_size_saturating(a, b);

    /* Postcondition: result is always a valid size_t (trivially true) */
    /* Postcondition: result <= SIZE_MAX (trivially true for size_t) */

    /* Postcondition: if overflow would occur, result must be SIZE_MAX */
    if (a > SIZE_MAX - b) {
        /* overflow case */
        assert(result == SIZE_MAX);
    } else {
        /* no overflow case */
        assert(result == a + b);
        assert(result >= a);
        assert(result >= b);
    }

    /* Postcondition: result is always >= min(a, b) or SIZE_MAX */
    assert(result == SIZE_MAX || result >= a);
    assert(result == SIZE_MAX || result >= b);
}
