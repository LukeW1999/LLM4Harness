#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

/**
 * Harness for aws_nospec_mask(size_t index, size_t bound)
 *
 * From the Doxygen:
 *   If index >= bound, bound > (SIZE_MAX / 2), or index > (SIZE_MAX / 2), returns 0.
 *   Otherwise, returns UINTPTR_MAX.
 *
 * Postconditions:
 *   1. If index < bound AND bound <= (SIZE_MAX / 2) AND index <= (SIZE_MAX / 2):
 *      result == UINTPTR_MAX
 *   2. Otherwise:
 *      result == 0
 */
void aws_nospec_mask_harness(void) {
    /* Non-deterministic inputs */
    size_t index = nondet_size_t();
    size_t bound = nondet_size_t();

    /* Call the function under test */
    size_t result = aws_nospec_mask(index, bound);

    /* Determine the expected outcome based on the Doxygen specification */
    int out_of_range = (index >= bound) || (bound > (SIZE_MAX / 2)) || (index > (SIZE_MAX / 2));

    if (out_of_range) {
        /* Any of the three conditions triggers a return of 0 */
        assert(result == 0);
    } else {
        /* All conditions satisfied: index < bound, bound <= SIZE_MAX/2, index <= SIZE_MAX/2 */
        assert(result == UINTPTR_MAX);
    }

    /* The result must always be either 0 or UINTPTR_MAX — no other value is valid */
    assert(result == 0 || result == UINTPTR_MAX);
}

void aws_nospec_mask_harness(void) {
    aws_nospec_mask_harness();
    return 0;
}
