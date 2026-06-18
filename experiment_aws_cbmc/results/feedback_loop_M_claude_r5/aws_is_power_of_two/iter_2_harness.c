#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_is_power_of_two_harness() {
    /* 1. Declare and bound inputs */
    size_t n;
    __CPROVER_assume(1); /* no constraint needed */

    /* 2. Call function under test */
    bool result = aws_is_power_of_two(n);

    /* 3. Assert postconditions */
    if (result) {
        /* n must be > 0 and have exactly one bit set */
        assert(n > 0);
        assert((n & (n - 1)) == 0);
    } else {
        /* n is either 0 or has more than one bit set */
        assert(n == 0 || (n & (n - 1)) != 0);
    }
}
