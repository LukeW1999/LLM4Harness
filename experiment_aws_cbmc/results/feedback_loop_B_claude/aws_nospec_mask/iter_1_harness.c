#include <aws/common/byte_buf.h>
#include <stdint.h>
#include <stddef.h>

/*
 * Harness for aws_nospec_mask(size_t index, size_t bound)
 *
 * The function computes a spectre-mitigation mask:
 *   - Returns UINTPTR_MAX (all ones) if index < bound AND neither index nor bound
 *     has the sign bit set (i.e., both < SIZE_MAX/2 + 1) AND bound <= SIZE_MAX/2
 *   - Returns 0 otherwise
 *
 * More precisely, the mask is all-ones iff:
 *   combined_mask = (index | bound) | (bound - index - 1)
 *   has its top bit clear, i.e., combined_mask < (SIZE_MAX/2 + 1)
 *
 * In-range condition (mask == UINTPTR_MAX):
 *   - index < bound
 *   - (index | bound) does not have sign bit set  => both index < SIZE_MAX/2+1 and bound < SIZE_MAX/2+1
 *   - (bound - index - 1) does not have sign bit set => bound > index (already covered) and no wrap
 *
 * Simplified: mask == UINTPTR_MAX iff index < bound AND bound <= SIZE_MAX/2
 * (because if bound <= SIZE_MAX/2 and index < bound, then index <= SIZE_MAX/2 - 1,
 *  so index | bound has top bit clear, and bound - index - 1 >= 0 with top bit clear)
 */

void aws_nospec_mask_harness(void) {
    size_t index = nondet_size_t();
    size_t bound = nondet_size_t();

    /* Call the function under test */
    size_t result = aws_nospec_mask(index, bound);

    /* The result must be either 0 or UINTPTR_MAX (all bits set) */
    assert(result == 0 || result == UINTPTR_MAX);

    /*
     * Determine the "in-range" condition as computed by the function:
     * combined_mask = (index | bound) | (bound - index - 1)
     * in_range iff top bit of combined_mask is 0
     */
    size_t negative_mask = index | bound;
    size_t toobig_mask = bound - index - (uintptr_t)1;
    size_t combined_mask = negative_mask | toobig_mask;

    size_t top_bit = SIZE_MAX - (SIZE_MAX >> 1);
    int in_range = ((combined_mask & top_bit) == 0);

    /* If in_range, result must be UINTPTR_MAX; otherwise 0 */
    if (in_range) {
        assert(result == UINTPTR_MAX);
    } else {
        assert(result == 0);
    }

    /*
     * Additional semantic checks:
     * If index >= bound, the mask must be 0 (no out-of-bounds access allowed)
     */
    if (index >= bound) {
        assert(result == 0);
    }

    /*
     * If bound has its sign bit set (bound > SIZE_MAX/2), mask must be 0
     * because negative_mask will have sign bit set.
     */
    if (bound & top_bit) {
        assert(result == 0);
    }

    /*
     * If index has its sign bit set, mask must be 0
     * because negative_mask will have sign bit set.
     */
    if (index & top_bit) {
        assert(result == 0);
    }

    /*
     * If index < bound AND bound <= SIZE_MAX/2 AND index <= SIZE_MAX/2,
     * then the mask should be UINTPTR_MAX.
     * (This is the "safe" in-range case.)
     */
    if (index < bound && !(bound & top_bit) && !(index & top_bit)) {
        /*
         * In this case, bound - index - 1 >= 0 and < bound <= SIZE_MAX/2,
         * so toobig_mask has no sign bit. combined_mask has no sign bit.
         * Therefore result == UINTPTR_MAX.
         */
        assert(result == UINTPTR_MAX);
    }
}
