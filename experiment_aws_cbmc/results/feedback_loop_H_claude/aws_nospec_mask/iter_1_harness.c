#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/* Declaration of the function under test */
size_t aws_nospec_mask(size_t index, size_t bound);

void aws_nospec_mask_harness(void) {
    /* 1. Declare non-deterministic inputs */
    size_t index = nondet_size_t();
    size_t bound = nondet_size_t();

    /* 2. Call function under test */
    size_t result = aws_nospec_mask(index, bound);

    /*
     * From the Doxygen description:
     * "If index >= bound, bound > (SIZE_MAX / 2), or index > (SIZE_MAX / 2), returns 0.
     *  Otherwise, returns UINTPTR_MAX."
     *
     * So we check both cases:
     */

    /* Case 1: Out-of-range conditions → result must be 0 */
    if (index >= bound || bound > (SIZE_MAX / 2) || index > (SIZE_MAX / 2)) {
        assert(result == 0);
    }

    /* Case 2: In-range conditions → result must be UINTPTR_MAX */
    if (index < bound && bound <= (SIZE_MAX / 2) && index <= (SIZE_MAX / 2)) {
        assert(result == UINTPTR_MAX);
    }

    /*
     * Combined: result must be either 0 or UINTPTR_MAX (no other values allowed)
     */
    assert(result == 0 || result == UINTPTR_MAX);

    /*
     * Verify the logical equivalence more precisely:
     * result == UINTPTR_MAX iff (index < bound && bound <= SIZE_MAX/2 && index <= SIZE_MAX/2)
     */
    if (result == UINTPTR_MAX) {
        assert(index < bound);
        assert(bound <= (SIZE_MAX / 2));
        assert(index <= (SIZE_MAX / 2));
    }

    if (result == 0) {
        /* At least one of the out-of-range conditions must hold */
        assert(index >= bound || bound > (SIZE_MAX / 2) || index > (SIZE_MAX / 2));
    }
}
