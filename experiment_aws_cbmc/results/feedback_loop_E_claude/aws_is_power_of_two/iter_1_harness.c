#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

/**
 * Harness for aws_is_power_of_two
 *
 * Doxygen: "Function to check if x is power of 2"
 * Implementation: return x && (!(x & (x - 1)));
 *
 * Postconditions:
 *   - Returns true if and only if x is a power of two (x != 0 and exactly one bit set)
 *   - Returns false if x == 0
 *   - Returns false if x has more than one bit set
 */
void aws_is_power_of_two_harness() {
    size_t x = nondet_size_t();

    bool result = aws_is_power_of_two(x);

    /* If x is 0, result must be false */
    if (x == 0) {
        assert(result == false);
    } else {
        /* x != 0: result is true iff exactly one bit is set */
        if (result) {
            /* Must have exactly one bit set: x & (x-1) == 0 */
            assert((x & (x - 1)) == 0);
            /* x must be non-zero */
            assert(x != 0);
        } else {
            /* result is false: either x == 0 (handled above) or more than one bit set */
            assert((x & (x - 1)) != 0);
        }
    }

    /* The function is pure and does not modify x */
    /* Verify consistency: result matches the expected formula */
    assert(result == (x != 0 && (x & (x - 1)) == 0));
}
