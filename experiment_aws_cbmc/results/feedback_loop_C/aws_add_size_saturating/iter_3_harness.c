#include <aws/common/common.h>
#include <aws/common/math.h>
#include <stddef.h>
#include <stdint.h>
#include <assert.h>

void aws_add_size_saturating_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t result = aws_add_size_saturating(a, b);

    if (result != SIZE_MAX) {
        // Success path
        assert(result == a + b);
    } else {
        // Failure path due to overflow
        __CPROVER_assume(a > SIZE_MAX - b); // Ensure overflow occurs
        assert(a + b < a);
    }
}
