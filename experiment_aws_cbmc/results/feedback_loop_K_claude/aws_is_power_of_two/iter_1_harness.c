#include <aws/common/math.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_is_power_of_two_harness(void) {
    size_t x;

    bool result = aws_is_power_of_two(x);

    /* Postcondition: result must equal the reference formula */
    assert(result == (x != 0 && (x & (x - 1)) == 0));

    /* Explicit case: x == 0 must return false */
    if (x == 0) {
        assert(result == false);
    }

    /* Explicit case: x == 1 must return true (1 is 2^0) */
    if (x == 1) {
        assert(result == true);
    }

    /* Explicit case: x == 2 must return true */
    if (x == 2) {
        assert(result == true);
    }

    /* Explicit case: x == 3 must return false */
    if (x == 3) {
        assert(result == false);
    }

    /* Explicit case: SIZE_MAX_POWER_OF_TWO must return true */
    if (x == SIZE_MAX_POWER_OF_TWO) {
        assert(result == true);
    }

    /* Explicit case: SIZE_MAX (all bits set, not a power of two) must return false */
    if (x == SIZE_MAX) {
        assert(result == false);
    }

    /* If result is true, then x must be nonzero */
    if (result) {
        assert(x != 0);
    }

    /* If result is true, then x & (x-1) must be zero */
    if (result) {
        assert((x & (x - 1)) == 0);
    }

    /* If result is false and x != 0, then x & (x-1) must be nonzero */
    if (!result && x != 0) {
        assert((x & (x - 1)) != 0);
    }
}
