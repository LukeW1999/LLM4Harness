#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>

void aws_nospec_mask_harness() {
    size_t index = nondet_size_t();
    size_t bound = nondet_size_t();

    size_t result = aws_nospec_mask(index, bound);

    /* Postcondition: result is either 0 or UINTPTR_MAX */
    assert(result == 0 || result == UINTPTR_MAX);

    /* Correctness: if all safe-index conditions hold, result must be UINTPTR_MAX;
     * otherwise it must be 0. */
    if (index < bound && bound <= SIZE_MAX / 2 && index <= SIZE_MAX / 2) {
        assert(result == UINTPTR_MAX);
    } else {
        assert(result == 0);
    }
}
