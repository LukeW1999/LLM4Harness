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

    /* 3. Assert postconditions based on Doxygen specification.
     *
     * Specification says:
     *   - If index >= bound, returns 0.
     *   - If bound > (SIZE_MAX / 2), returns 0.
     *   - If index > (SIZE_MAX / 2), returns 0.
     *   - Otherwise, returns UINTPTR_MAX.
     */

    if (index >= bound || bound > (SIZE_MAX / 2) || index > (SIZE_MAX / 2)) {
        assert(result == 0);
    } else {
        assert(result == UINTPTR_MAX);
    }

    /* 4. The function returns a mask that is either 0 or UINTPTR_MAX.
     *    This is a pure mathematical function with no side effects,
     *    so no validity invariants or struct checks are needed.
     */
}
