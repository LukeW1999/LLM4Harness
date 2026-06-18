#include <aws/common/common.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <limits.h>

void aws_mul_size_checked_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    size_t result;
    size_t old_result = nondet_size_t();
    result = old_result;

    bool overflow = aws_mul_size_checked(a, b, &result);

    if (overflow) {
        /* Overflow must have occurred */
        assert(a != 0 && b != 0);
        assert(a > SIZE_MAX / b);
        /* Result must be unchanged */
        assert(result == old_result);
    } else {
        /* No overflow: result equals product */
        assert(a == 0 || b == 0 || a <= SIZE_MAX / b);
        assert(result == a * b);
    }
}
