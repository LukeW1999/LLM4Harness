#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_is_power_of_two_harness() {
    /* 1. Declare inputs */
    size_t n;
    __CPROVER_assume(true); /* no constraint */

    /* 2. Call function under test */
    bool result = aws_is_power_of_two(n);

    /* 3. Assert postconditions */
    if (n == 0) {
        /* 0 is not a power of two */
        assert(!result);
    } else if (result) {
        /* If result is true, n must have exactly one bit set */
        assert((n & (n - 1)) == 0);
    } else {
        /* If result is false, n must have more than one bit set */
        assert((n & (n - 1)) != 0);
    }
}
