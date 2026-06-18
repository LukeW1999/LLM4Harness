#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <aws/common/linked_list.h>
#include <aws/common/math.h>
#include <aws/common/string.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_nospec_mask_harness(void) {
    /* 1. Non‑deterministic inputs */
    size_t index = nondet_size_t();
    size_t bound = nondet_size_t();

    /* 2. Save old state */
    size_t old_index = index;
    size_t old_bound = bound;

    /* 3. Call function under test */
    size_t result = aws_nospec_mask(index, bound);

    /* 4. Post‑conditions */

    /* Unchanged inputs */
    assert(index == old_index);
    assert(bound == old_bound);

    /* Return value matches specification */
    if (index >= bound || bound > (SIZE_MAX / 2) || index > (SIZE_MAX / 2)) {
        assert(result == 0);
    } else {
        assert(result == UINTPTR_MAX);
    }

    /* Result is always either 0 or all‑ones */
    assert(result == 0 || result == UINTPTR_MAX);
}
