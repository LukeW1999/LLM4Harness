#include <aws/common/math.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_is_power_of_two_harness(void) {
    size_t x;

    bool result = aws_is_power_of_two(x);

    /* Postcondition 1: Return value correctness
     * aws_is_power_of_two returns true iff x is a power of two (nonzero and only one bit set) */
    if (x == 0) {
        /* 0 is not a power of two */
        assert(result == false);
    } else {
        /* For nonzero x, it's a power of two iff (x & (x-1)) == 0 */
        if ((x & (x - 1)) == 0) {
            assert(result == true);
        } else {
            assert(result == false);
        }
    }

    /* Postcondition 2: Specific known values */
    /* If x == 1, result must be true (1 = 2^0) */
    if (x == 1) {
        assert(result == true);
    }

    /* If x == 2, result must be true (2 = 2^1) */
    if (x == 2) {
        assert(result == true);
    }

    /* If x == 3, result must be false */
    if (x == 3) {
        assert(result == false);
    }

    /* If x == 4, result must be true (4 = 2^2) */
    if (x == 4) {
        assert(result == true);
    }

    /* Postcondition 3: If result is true, x must be nonzero */
    if (result == true) {
        assert(x != 0);
    }

    /* Postcondition 4: If result is true, x & (x-1) must be 0 */
    if (result == true) {
        assert((x & (x - 1)) == 0);
    }

    /* Postcondition 5: If result is false and x != 0, then x & (x-1) must be nonzero */
    if (result == false && x != 0) {
        assert((x & (x - 1)) != 0);
    }

    /* Postcondition 6: SIZE_MAX_POWER_OF_TWO is a power of two */
    bool max_pow2_result = aws_is_power_of_two(SIZE_MAX_POWER_OF_TWO);
    assert(max_pow2_result == true);

    /* Postcondition 7: Frame condition - the function is pure (no side effects),
     * so x itself should not be modified */
    size_t x_copy = x;
    (void)aws_is_power_of_two(x);
    assert(x == x_copy);
}
