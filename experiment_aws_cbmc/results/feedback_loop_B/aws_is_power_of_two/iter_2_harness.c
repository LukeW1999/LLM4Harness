#include <aws/common/common.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>

void aws_is_power_of_two_harness() {
    /* 1. Declare and bound data structures */
    size_t n = nondet_size_t();

    /* 2. Call function under test */
    bool result = aws_is_power_of_two(n);

    /* 3. Assert postconditions for BOTH success and failure paths */
    if (result) {
        assert(n > 0);
        assert((n & (n - 1)) == 0);  // n is a power of two
    } else {
        assert(n == 0 || (n & (n - 1)) != 0);  // n is not a power of two
    }

    /* 4. Assert validity invariants always holds */
    // No additional validity invariants for primitive types
}
