#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_nospec_mask_harness() {
    /* 1. Declare non-deterministic inputs */
    size_t index = nondet_size_t();
    size_t bound = nondet_size_t();

    /* 2. Call function under test */
    size_t result = aws_nospec_mask(index, bound);

    /* 3. Assert postconditions */
    /*
     * Doxygen says:
     * If index >= bound, bound > (SIZE_MAX / 2), or index > (SIZE_MAX / 2),
     * returns 0.
     * Otherwise, returns UINTPTR_MAX.
     *
     * UINTPTR_MAX is SIZE_MAX on systems where sizeof(size_t) == sizeof(uintptr_t).
     * Check all conditions.
     */

    /*
     * Condition 1: bound > (SIZE_MAX / 2)
     * OR Condition 2: index > (SIZE_MAX / 2)
     */
    if (bound > (SIZE_MAX / 2) || index > (SIZE_MAX / 2)) {
        assert(result == 0);
    } else if (index >= bound) {
        /* Condition 3: index >= bound */
        assert(result == 0);
    } else {
        /* Otherwise: index < bound, bound <= SIZE_MAX/2, index <= SIZE_MAX/2 */
        assert(result == UINTPTR_MAX);
    }
}
