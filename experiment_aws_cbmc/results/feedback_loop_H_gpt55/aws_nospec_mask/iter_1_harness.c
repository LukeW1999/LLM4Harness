#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

size_t aws_nospec_mask(size_t index, size_t bound);

void aws_nospec_mask_harness() {
    size_t index = nondet_size_t();
    size_t bound = nondet_size_t();

    size_t old_index = index;
    size_t old_bound = bound;

    size_t result = aws_nospec_mask(index, bound);

    if (index >= bound || bound > (SIZE_MAX / 2) || index > (SIZE_MAX / 2)) {
        assert(result == 0);
    } else {
        assert(result == (size_t)UINTPTR_MAX);
    }

    assert(result == 0 || result == (size_t)UINTPTR_MAX);

    assert(index == old_index);
    assert(bound == old_bound);

    assert(true);
}
