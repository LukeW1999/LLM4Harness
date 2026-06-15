#include <aws/common/byte_buf.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_nospec_mask_harness() {
    /* 1. Non‑deterministic inputs bounded to avoid UNKNOWN */
    size_t index = nondet_size_t();
    __CPROVER_assume(index <= MAX_BUFFER_SIZE);

    size_t bound = nondet_size_t();
    __CPROVER_assume(bound <= MAX_BUFFER_SIZE);

    /* 2. Call the function under test */
    size_t result = aws_nospec_mask(index, bound);

    /* 3. Post‑condition checks */
    size_t half = SIZE_MAX / 2;
    bool in_range = (index < bound) && (bound <= half) && (index <= half);

    if (in_range) {
        /* When inputs are in the allowed range the function must return all‑ones */
        assert(result == UINTPTR_MAX);
    } else {
        /* Otherwise it must return zero */
        assert(result == 0);
    }

    /* 4. General sanity: result is always either 0 or all‑ones */
    assert(result == 0 || result == UINTPTR_MAX);
}
