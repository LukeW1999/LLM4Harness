#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* Declaration of the function under test */
size_t aws_nospec_mask(size_t index, size_t bound);

void aws_nospec_mask_harness(void) {
    /* 1. Declare non-deterministic inputs */
    size_t index = nondet_size_t();
    size_t bound = nondet_size_t();

    /* Save inputs before call (frame conditions) */
    size_t old_index = index;
    size_t old_bound = bound;

    /* 2. Call function under test */
    size_t result = aws_nospec_mask(index, bound);

    /* 3. Assert frame conditions: inputs must not change */
    assert(index == old_index);
    assert(bound == old_bound);

    /* 4. Assert postconditions based on Doxygen specification:
     *
     * "If index >= bound, bound > (SIZE_MAX / 2), or index > (SIZE_MAX / 2),
     *  returns 0. Otherwise, returns UINTPTR_MAX."
     */

    /* Determine the expected result based on the specification */
    bool out_of_range = (index >= bound) ||
                        (bound > (SIZE_MAX / 2)) ||
                        (index > (SIZE_MAX / 2));

    if (out_of_range) {
        /* Any of the three conditions triggers a return of 0 */
        assert(result == 0);
    } else {
        /* All conditions are false: index < bound, bound <= SIZE_MAX/2,
         * index <= SIZE_MAX/2 → returns UINTPTR_MAX */
        assert(result == UINTPTR_MAX);
    }

    /* 5. Assert that result is always either 0 or UINTPTR_MAX (binary mask) */
    assert(result == 0 || result == UINTPTR_MAX);

    /* 6. Verify the specific boundary conditions explicitly */

    /* If index >= bound, result must be 0 */
    if (index >= bound) {
        assert(result == 0);
    }

    /* If bound > SIZE_MAX/2, result must be 0 */
    if (bound > (SIZE_MAX / 2)) {
        assert(result == 0);
    }

    /* If index > SIZE_MAX/2, result must be 0 */
    if (index > (SIZE_MAX / 2)) {
        assert(result == 0);
    }

    /* If none of the out-of-range conditions hold, result must be UINTPTR_MAX */
    if (index < bound && bound <= (SIZE_MAX / 2) && index <= (SIZE_MAX / 2)) {
        assert(result == UINTPTR_MAX);
    }
}
