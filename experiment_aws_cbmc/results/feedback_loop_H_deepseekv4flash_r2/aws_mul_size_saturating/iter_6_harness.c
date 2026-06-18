#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_mul_size_saturating_harness() {
    size_t a;
    size_t b;
    /* Bound inputs to keep verification time reasonable */
    __CPROVER_assume(a <= 1000);
    __CPROVER_assume(b <= 1000);

    size_t result = aws_mul_size_saturating(a, b);

    /* Detect overflow using division: overflow if b != 0 and a > SIZE_MAX / b */
    bool overflow = (b != 0 && a > SIZE_MAX / b);
    if (overflow) {
        assert(result == SIZE_MAX);
    } else {
        assert(result == a * b);
    }
}
