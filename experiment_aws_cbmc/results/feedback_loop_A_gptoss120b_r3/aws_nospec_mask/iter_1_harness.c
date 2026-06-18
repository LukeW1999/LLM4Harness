#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

void aws_nospec_mask_harness(void) {
    size_t index = nondet_size_t();
    size_t bound = nondet_size_t();

    size_t result = aws_nospec_mask(index, bound);

    /* Postcondition: result must be either 0 or UINTPTR_MAX */
    assert(result == 0 || result == UINTPTR_MAX);

    if (index >= bound || bound > (SIZE_MAX / 2) || index > (SIZE_MAX / 2)) {
        assert(result == 0);
    } else {
        assert(result == UINTPTR_MAX);
    }
}
