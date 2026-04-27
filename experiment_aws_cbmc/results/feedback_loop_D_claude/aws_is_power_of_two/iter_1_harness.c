// === STEP 1: SUCCESS PATH ===
// aws_is_power_of_two is a boolean function (returns bool), not AWS_OP_SUCCESS/ERR
// When aws_is_power_of_two returns true:
//   - x != 0 AND (x & (x - 1)) == 0
//   - i.e., x is a power of two
//
// === STEP 2: FAILURE PATH ===
// When aws_is_power_of_two returns false:
//   - x == 0 OR (x & (x - 1)) != 0
//   - i.e., x is zero or not a power of two
//
// === STEP 3: FRAME CONDITIONS ===
// No struct parameters. Only a single size_t input x.
// The function is pure (no side effects, no output pointers).
// x: UNCHANGED (passed by value)
//
// === STEP 4: VALIDITY INVARIANTS ===
// The function returns true iff x != 0 && (x & (x-1)) == 0
// Key properties to verify:
//   - If result is true, then x != 0
//   - If result is true, then (x & (x-1)) == 0
//   - If x == 0, result must be false
//   - If x == 1, result must be true (1 is 2^0)
//   - If x is a power of two, result must be true
//   - Result matches the manual computation: x && !(x & (x-1))

#include <aws/common/math.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

void aws_is_power_of_two_harness(void) {
    /* Nondeterministic input */
    size_t x = nondet_size_t();

    /* Call the function under test */
    bool result = aws_is_power_of_two(x);

    /* === Encode Step 1 & 2: Correctness of return value === */

    /* The function returns true iff x != 0 AND (x & (x-1)) == 0 */
    bool expected = (x != 0) && ((x & (x - 1)) == 0);
    assert(result == expected);

    /* If result is true, x must be non-zero */
    if (result) {
        assert(x != 0);
        assert((x & (x - 1)) == 0);
    }

    /* If result is false, either x == 0 or x has more than one bit set */
    if (!result) {
        assert(x == 0 || (x & (x - 1)) != 0);
    }

    /* === Specific boundary cases (always checked via the general formula above,
       but we add explicit checks for clarity) === */

    /* x == 0 must return false */
    if (x == 0) {
        assert(result == false);
    }

    /* x == 1 (2^0) must return true */
    if (x == 1) {
        assert(result == true);
    }

    /* x == SIZE_MAX_POWER_OF_TWO (largest power of two for size_t) must return true */
    /* SIZE_MAX_POWER_OF_TWO = ((size_t)1) << (SIZE_BITS - 1) */
    size_t max_pow2 = ((size_t)1) << (sizeof(size_t) * 8 - 1);
    if (x == max_pow2) {
        assert(result == true);
    }

    /* SIZE_MAX is not a power of two (all bits set), must return false */
    if (x == SIZE_MAX) {
        assert(result == false);
    }
}
