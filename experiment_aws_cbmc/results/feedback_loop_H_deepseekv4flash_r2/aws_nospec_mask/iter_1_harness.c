#include <aws/common/private/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>

void aws_nospec_mask_harness() {
    size_t index;
    size_t bound;

    // Call the function
    size_t mask = aws_nospec_mask(index, bound);

    // Postcondition as per specification
    if (index >= bound || bound > (SIZE_MAX / 2) || index > (SIZE_MAX / 2)) {
        assert(mask == 0);
    } else {
        assert(mask == UINTPTR_MAX);
    }

    // Additionally, mask must be either 0 or UINTPTR_MAX
    assert(mask == 0 || mask == UINTPTR_MAX);
}
