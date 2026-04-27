#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

/**
 * Harness for aws_mul_size_saturating
 *
 * Doxygen: Multiplies a * b. If the result overflows, returns SIZE_MAX.
 *
 * Postconditions:
 * - If a * b does not overflow, result == a * b
 * - If a * b overflows, result == SIZE_MAX
 */
void aws_mul_size_saturating_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    size_t result = aws_mul_size_saturating(a, b);

#if SIZE_MAX == UINT64_MAX
    /* Check for overflow using 128-bit arithmetic or the standard overflow check */
    if (a == 0 || b == 0) {
        assert(result == 0);
    } else if (a > SIZE_MAX / b) {
        /* overflow */
        assert(result == SIZE_MAX);
    } else {
        /* no overflow */
        assert(result == a * b);
    }
#elif SIZE_MAX == UINT32_MAX
    if (a == 0 || b == 0) {
        assert(result == 0);
    } else if (a > SIZE_MAX / b) {
        /* overflow */
        assert(result == SIZE_MAX);
    } else {
        /* no overflow */
        assert(result == a * b);
    }
#endif
}
