#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_mul_size_saturating_harness(void) {
    size_t a = (size_t)nondet_uint64_t();
    size_t b = (size_t)nondet_uint64_t();

    size_t result = aws_mul_size_saturating(a, b);

    if (b != 0 && a > SIZE_MAX / b) {
        /* Overflow case */
        __CPROVER_assert(result == SIZE_MAX, "Result must be SIZE_MAX on overflow");
        __CPROVER_assert(a > SIZE_MAX / b, "a must be greater than SIZE_MAX / b when overflow occurs");
    } else {
        /* No overflow */
        __CPROVER_assert(result == a * b, "Result must be exact product when no overflow");
        if (b != 0) {
            __CPROVER_assert(a <= SIZE_MAX / b, "a must be <= SIZE_MAX / b when no overflow");
        }
    }
}
