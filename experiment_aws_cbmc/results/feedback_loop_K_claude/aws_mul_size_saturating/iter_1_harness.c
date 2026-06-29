#include <aws/common/math.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

void aws_mul_size_saturating_harness(void) {
    size_t a;
    size_t b;

    /* No preconditions needed - function must handle all size_t inputs */

    size_t result = aws_mul_size_saturating(a, b);

    /* Postcondition 1: result is either SIZE_MAX or the exact product */
    /* Check overflow: if a != 0 and result / a != b, then overflow occurred */
    bool overflow = (a != 0) && (result / a != b);

    if (overflow) {
        /* If overflow occurred, result must be SIZE_MAX */
        assert(result == SIZE_MAX);
    } else {
        /* If no overflow, result must equal a * b */
        assert(result == a * b);
    }

    /* Postcondition 2: result is always <= SIZE_MAX (trivially true for size_t, but explicit) */
    assert(result <= SIZE_MAX);

    /* Postcondition 3: if either operand is 0, result must be 0 */
    if (a == 0 || b == 0) {
        assert(result == 0);
    }

    /* Postcondition 4: if a == 1, result must equal b (no overflow possible unless b == SIZE_MAX) */
    if (a == 1) {
        assert(result == b);
    }

    /* Postcondition 5: if b == 1, result must equal a */
    if (b == 1) {
        assert(result == a);
    }

    /* Postcondition 6: result is either the exact product or SIZE_MAX */
    assert(result == SIZE_MAX || result == a * b);

    /* Postcondition 7: saturating means result >= a * b is not required,
       but result must not be less than a*b unless overflow happened */
    /* If no overflow detected via division check, result == a*b */
    if (a != 0 && b != 0) {
        /* Use 128-bit or double-check: if result < SIZE_MAX then no overflow */
        if (result < SIZE_MAX) {
            assert(result == a * b);
            /* Verify no overflow actually occurred */
            assert(result / a == b);
        }
    }
}
