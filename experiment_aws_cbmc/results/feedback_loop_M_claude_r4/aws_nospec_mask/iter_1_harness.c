#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>

void aws_nospec_mask_harness() {
    /* 1. Declare and bound scalar inputs */
    size_t index = nondet_size_t();
    size_t bound = nondet_size_t();

    /* No need to bound these further — the function handles all cases including
     * overflow, and the computation is purely arithmetic with no loops or
     * memory accesses. CBMC can handle unbounded size_t arithmetic here. */

    /* 2. Call function under test */
    size_t result = aws_nospec_mask(index, bound);

    /* 3. Assert postconditions based on the Doxygen specification:
     *
     * "If index >= bound, bound > (SIZE_MAX / 2), or index > (SIZE_MAX / 2),
     *  returns 0. Otherwise, returns UINTPTR_MAX."
     */

    /* Case 1: Any out-of-range condition → result must be 0 */
    if (index >= bound || bound > (SIZE_MAX / 2) || index > (SIZE_MAX / 2)) {
        assert(result == 0);
    }
    /* Case 2: All conditions satisfied → result must be UINTPTR_MAX */
    else {
        assert(result == UINTPTR_MAX);
    }

    /* 4. Result must be either 0 or UINTPTR_MAX — no other values are valid */
    assert(result == 0 || result == UINTPTR_MAX);
}
