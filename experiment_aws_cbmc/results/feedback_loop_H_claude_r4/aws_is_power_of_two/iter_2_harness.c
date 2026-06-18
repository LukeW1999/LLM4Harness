#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_is_power_of_two_harness() {
    /* 1. Declare inputs */
    size_t n;
    __CPROVER_assume(true);

    /* 2. Call function under test */
    bool result = aws_is_power_of_two(n);

    /* 3. Assert postconditions */
    if (n == 0) {
        assert(!result);
    } else if ((n & (n - 1)) == 0) {
        assert(result);
    } else {
        assert(!result);
    }
}
