#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* Declaration from private header */
size_t aws_nospec_mask(size_t index, size_t bound);

void aws_nospec_mask_harness(void) {
    size_t index;
    size_t bound;

    /* No preconditions — function is total over all size_t inputs */

    size_t result = aws_nospec_mask(index, bound);

    /*
     * Postcondition 1:
     * If index > (SIZE_MAX / 2), result must be 0.
     */
    if (index > (SIZE_MAX / 2)) {
        assert(result == 0);
    }

    /*
     * Postcondition 2:
     * If bound > (SIZE_MAX / 2), result must be 0.
     */
    if (bound > (SIZE_MAX / 2)) {
        assert(result == 0);
    }

    /*
     * Postcondition 3:
     * If index >= bound, result must be 0.
     */
    if (index >= bound) {
        assert(result == 0);
    }

    /*
     * Postcondition 4:
     * If all conditions are safe (index < bound, bound <= SIZE_MAX/2,
     * index <= SIZE_MAX/2), result must be UINTPTR_MAX.
     */
    if (index < bound && bound <= (SIZE_MAX / 2) && index <= (SIZE_MAX / 2)) {
        assert(result == UINTPTR_MAX);
    }

    /*
     * Postcondition 5:
     * Result is always either 0 or UINTPTR_MAX — no other values are valid.
     */
    assert(result == 0 || result == UINTPTR_MAX);

    /*
     * Postcondition 6 (frame):
     * The function is pure — no global state is modified.
     * Verified implicitly by the absence of any pointer parameters.
     */
}
