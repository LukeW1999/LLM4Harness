#include <aws/common/private/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_nospec_mask_harness() {
    size_t index = nondet_size_t();
    size_t bound = nondet_size_t();

    // Precondition: index >= bound, bound > (SIZE_MAX / 2), or index > (SIZE_MAX / 2) should return 0
    // Otherwise, it should return UINTPTR_MAX
    __CPROVER_assume(!(index >= bound && bound > (SIZE_MAX / 2) && index > (SIZE_MAX / 2)));

    size_t result = aws_nospec_mask(index, bound);

    if (index >= bound || bound > (SIZE_MAX / 2) || index > (SIZE_MAX / 2)) {
        assert(result == 0);
    } else {
        assert(result == UINTPTR_MAX);
    }
}
