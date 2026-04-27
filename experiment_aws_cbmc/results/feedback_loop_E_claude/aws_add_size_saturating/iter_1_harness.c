#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

/**
 * Harness for aws_add_size_saturating
 *
 * Doxygen: Adds a + b. If the result overflows returns SIZE_MAX.
 *
 * Postconditions:
 * - If a + b does not overflow, result == a + b
 * - If a + b overflows, result == SIZE_MAX
 */
void aws_add_size_saturating_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    size_t result = aws_add_size_saturating(a, b);

    /* Check overflow condition */
    if (b > 0 && a > (SIZE_MAX - b)) {
        /* Overflow: result must be SIZE_MAX */
        assert(result == SIZE_MAX);
    } else {
        /* No overflow: result must equal a + b */
        assert(result == a + b);
    }
}
