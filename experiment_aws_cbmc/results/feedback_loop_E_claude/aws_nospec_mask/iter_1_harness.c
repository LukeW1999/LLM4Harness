#include <aws/common/byte_buf.h>
#include <aws/common/private/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <stddef.h>

/**
 * Harness for aws_nospec_mask
 *
 * From the Doxygen:
 *   If index >= bound, bound > (SIZE_MAX / 2), or index > (SIZE_MAX / 2), returns 0.
 *   Otherwise, returns UINTPTR_MAX.
 *
 * Postconditions:
 *   - If index < bound AND bound <= (SIZE_MAX / 2) AND index <= (SIZE_MAX / 2):
 *       result == UINTPTR_MAX
 *   - Otherwise:
 *       result == 0
 */
void aws_nospec_mask_harness() {
    size_t index = nondet_size_t();
    size_t bound = nondet_size_t();

    size_t result = aws_nospec_mask(index, bound);

    /* Determine which case we are in */
    int out_of_range = (index >= bound) || (bound > (SIZE_MAX / 2)) || (index > (SIZE_MAX / 2));

    if (!out_of_range) {
        /* In-range: index < bound, bound <= SIZE_MAX/2, index <= SIZE_MAX/2 */
        assert(result == UINTPTR_MAX);
    } else {
        /* Out-of-range */
        assert(result == 0);
    }
}
