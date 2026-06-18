#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

/* Declaration of the function under test */
size_t aws_nospec_mask(size_t index, size_t bound);

void aws_nospec_mask_harness(void) {
    /* 1. Declare non-deterministic inputs */
    size_t index = nondet_size_t();
    size_t bound = nondet_size_t();

    /* 2. Call the function under test */
    size_t result = aws_nospec_mask(index, bound);

    /*
     * From the Doxygen description:
     * "If index >= bound, bound > (SIZE_MAX / 2), or index > (SIZE_MAX / 2),
     *  returns 0. Otherwise, returns UINTPTR_MAX."
     *
     * Postconditions:
     * - If index >= bound: result == 0
     * - If bound > (SIZE_MAX / 2): result == 0
     * - If index > (SIZE_MAX / 2): result == 0
     * - Otherwise (all conditions false): result == UINTPTR_MAX
     */

    /* Check the "returns 0" conditions */
    if (index >= bound) {
        assert(result == 0);
    } else if (bound > (SIZE_MAX / 2)) {
        assert(result == 0);
    } else if (index > (SIZE_MAX / 2)) {
        assert(result == 0);
    } else {
        /* All conditions false: should return UINTPTR_MAX */
        assert(result == UINTPTR_MAX);
    }

    /* The result must be either 0 or UINTPTR_MAX (it's a mask) */
    assert(result == 0 || result == UINTPTR_MAX);

    /*
     * Additional combined check: if none of the "bad" conditions hold,
     * result must be UINTPTR_MAX.
     */
    if (index < bound && bound <= (SIZE_MAX / 2) && index <= (SIZE_MAX / 2)) {
        assert(result == UINTPTR_MAX);
    }

    /*
     * Converse: if result == UINTPTR_MAX, then none of the bad conditions hold.
     */
    if (result == UINTPTR_MAX) {
        assert(index < bound);
        assert(bound <= (SIZE_MAX / 2));
        assert(index <= (SIZE_MAX / 2));
    }

    /*
     * If result == 0, at least one bad condition must hold.
     */
    if (result == 0) {
        assert(index >= bound || bound > (SIZE_MAX / 2) || index > (SIZE_MAX / 2));
    }
}
