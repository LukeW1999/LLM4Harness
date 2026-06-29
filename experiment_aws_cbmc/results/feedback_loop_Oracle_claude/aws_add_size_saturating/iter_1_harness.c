#include <aws/common/math.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_add_size_saturating_harness(void) {
    size_t a;
    size_t b;

    /* Call the function under test */
    size_t result = aws_add_size_saturating(a, b);

    /* Postcondition 1: If overflow would occur, result must be SIZE_MAX */
    if (a > SIZE_MAX - b) {
        /* Overflow case: result must be saturated to SIZE_MAX */
        assert(result == SIZE_MAX);
    } else {
        /* No overflow case: result must equal a + b */
        assert(result == a + b);
    }

    /* Postcondition 2: Result is always <= SIZE_MAX (trivially true for size_t, but explicit) */
    assert(result <= SIZE_MAX);

    /* Postcondition 3: Result is always >= a (since b >= 0 for size_t, and saturation only increases) */
    /* If no overflow, result == a + b >= a. If overflow, result == SIZE_MAX >= a */
    assert(result >= a);

    /* Postcondition 4: Result is always >= b */
    assert(result >= b);

    /* Postcondition 5: If both a and b are 0, result is 0 */
    if (a == 0 && b == 0) {
        assert(result == 0);
    }

    /* Postcondition 6: If a == 0, result == b (no overflow possible when a == 0) */
    if (a == 0) {
        assert(result == b);
    }

    /* Postcondition 7: If b == 0, result == a (no overflow possible when b == 0) */
    if (b == 0) {
        assert(result == a);
    }

    /* Postcondition 8: Saturation correctness - result is either exact sum or SIZE_MAX */
    assert(result == a + b || result == SIZE_MAX);

    /* Postcondition 9: If result != SIZE_MAX, then no overflow occurred and result == a + b */
    if (result != SIZE_MAX) {
        assert(a <= SIZE_MAX - b);
        assert(result == a + b);
    }

    /* Postcondition 10: If a == SIZE_MAX, result must be SIZE_MAX (since b >= 0) */
    if (a == SIZE_MAX) {
        assert(result == SIZE_MAX);
    }

    /* Postcondition 11: If b == SIZE_MAX, result must be SIZE_MAX (since a >= 0) */
    if (b == SIZE_MAX) {
        assert(result == SIZE_MAX);
    }
}
