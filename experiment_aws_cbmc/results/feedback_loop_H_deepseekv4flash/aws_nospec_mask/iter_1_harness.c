#include <aws/common/byte_buf.h>
#include <stdint.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_nospec_mask_harness() {
    size_t index = nondet_size_t();
    size_t bound = nondet_size_t();

    /* Call the function under test */
    size_t result = aws_nospec_mask(index, bound);

    /* Postcondition based on specification */
    /* If index >= bound, bound > (SIZE_MAX / 2), or index > (SIZE_MAX / 2), returns 0.
       Otherwise, returns UINTPTR_MAX. */
    if (index >= bound || bound > (SIZE_MAX / 2) || index > (SIZE_MAX / 2)) {
        assert(result == 0);
    } else {
        assert(result == UINTPTR_MAX);
    }
}
