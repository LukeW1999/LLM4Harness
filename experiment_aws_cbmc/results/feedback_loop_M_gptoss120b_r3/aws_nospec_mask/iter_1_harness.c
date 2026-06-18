#include <aws/common/byte_buf.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

void aws_nospec_mask_harness() {
    /* 1. Non‑deterministic inputs bounded to avoid UNKNOWN */
    size_t index = nondet_size_t();
    __CPROVER_assume(index <= MAX_BUFFER_SIZE);

    size_t bound = nondet_size_t();
    __CPROVER_assume(bound <= MAX_BUFFER_SIZE);

    /* 2. Call the function under test */
    size_t result = aws_nospec_mask(index, bound);

    /* 3. Post‑condition derived from the Doxygen description */
    bool in_range = !(index >= bound ||
                     bound > (SIZE_MAX / 2) ||
                     index > (SIZE_MAX / 2));

    if (in_range) {
        /* When the index is within bounds, the mask should be all‑ones */
        assert(result == UINTPTR_MAX);
    } else {
        /* Out‑of‑range cases must yield a zero mask */
        assert(result == 0);
    }

    /* 4. General sanity: result is always either 0 or all‑ones */
    assert(result == 0 || result == UINTPTR_MAX);
}
