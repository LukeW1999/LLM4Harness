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
    /* 1. Declare inputs non-deterministically */
    size_t index = nondet_size_t();
    size_t bound = nondet_size_t();

    /* 2. Call function under test */
    size_t result = aws_nospec_mask(index, bound);

    /*
     * From the Doxygen description:
     * "If index >= bound, bound > (SIZE_MAX / 2), or index > (SIZE_MAX / 2), returns 0.
     *  Otherwise, returns UINTPTR_MAX."
     *
     * Postconditions:
     * - If index >= bound OR bound > (SIZE_MAX/2) OR index > (SIZE_MAX/2): result == 0
     * - Otherwise (index < bound AND bound <= SIZE_MAX/2 AND index <= SIZE_MAX/2): result == UINTPTR_MAX
     */

    size_t half_max = SIZE_MAX / 2;

    if (index >= bound || bound > half_max || index > half_max) {
        assert(result == 0);
    } else {
        assert(result == UINTPTR_MAX);
    }

    /* The result must be either 0 or UINTPTR_MAX — no other values are valid */
    assert(result == 0 || result == UINTPTR_MAX);
}
