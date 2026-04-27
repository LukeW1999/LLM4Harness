// === STEP 1: SUCCESS PATH ===
// aws_nospec_mask returns a size_t mask value (not AWS_OP_SUCCESS/ERR)
// When index < bound AND index <= SIZE_MAX/2 AND bound <= SIZE_MAX/2:
//   - returns UINTPTR_MAX (all bits set)
// Otherwise:
//   - returns 0
//
// === STEP 2: FAILURE PATH ===
// This function has no failure path in the traditional sense.
// It returns 0 when: index >= bound, OR bound > SIZE_MAX/2, OR index > SIZE_MAX/2
// It returns UINTPTR_MAX otherwise.
// No struct parameters are modified.
//
// === STEP 3: FRAME CONDITIONS ===
// No struct parameters - function takes two size_t values and returns size_t.
// No memory is modified by this function.
//
// === STEP 4: VALIDITY INVARIANTS ===
// The function should return either 0 or UINTPTR_MAX.
// Specifically:
//   - If index >= bound OR bound > SIZE_MAX/2 OR index > SIZE_MAX/2: result == 0
//   - Otherwise: result == UINTPTR_MAX

#include <aws/common/private/byte_buf.h>
#include <aws/common/byte_buf.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>

void aws_nospec_mask_harness(void) {
    /* Non-deterministic inputs */
    size_t index = nondet_size_t();
    size_t bound = nondet_size_t();

    /* Call the function under test */
    size_t result = aws_nospec_mask(index, bound);

    /* The result must be either 0 or UINTPTR_MAX - no other values are valid */
    assert(result == 0 || result == UINTPTR_MAX);

    /* Determine the "safe" condition:
     * index <= SIZE_MAX/2 AND bound <= SIZE_MAX/2 AND index < bound
     * Note: SIZE_MAX/2 == SIZE_MAX >> 1
     */
    size_t half_max = SIZE_MAX >> 1;

    int index_in_range = (index <= half_max);
    int bound_in_range = (bound <= half_max);
    int index_lt_bound = (index < bound);

    if (index_in_range && bound_in_range && index_lt_bound) {
        /* All conditions met: should return UINTPTR_MAX */
        assert(result == UINTPTR_MAX);
    } else {
        /* At least one condition violated: should return 0 */
        assert(result == 0);
    }

    /* Verify the contrapositive: if result is UINTPTR_MAX, all conditions hold */
    if (result == UINTPTR_MAX) {
        assert(index <= half_max);
        assert(bound <= half_max);
        assert(index < bound);
    }

    /* Verify: if result is 0, at least one condition is violated */
    if (result == 0) {
        assert(!index_in_range || !bound_in_range || !index_lt_bound);
    }
}
