#include <aws/common/private/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>

void aws_nospec_mask_harness() {
    size_t index = nondet_size_t();
    size_t bound = nondet_size_t();

    size_t result = aws_nospec_mask(index, bound);

    size_t expected;
    if (index >= bound || bound > (SIZE_MAX / 2) || index > (SIZE_MAX / 2)) {
        expected = 0;
    } else {
        expected = UINTPTR_MAX;
    }

    assert(result == expected);
}
