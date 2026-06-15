#include <aws/common/byte_buf.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <limits.h>
#include <proof_helpers/nondet.h>

void aws_nospec_mask_harness(void) {
    /* 1. Non‑deterministic inputs */
    size_t index = nondet_size_t();
    size_t bound = nondet_size_t();

    /* 2. Call the function under test */
    size_t result = aws_nospec_mask(index, bound);

    /* 3. Post‑condition checks */
    size_t half = SIZE_MAX / 2;
    bool out_of_range = (index >= bound) || (bound > half) || (index > half);

    if (out_of_range) {
        /* When any out‑of‑range condition holds, the mask must be zero */
        assert(result == 0);
    } else {
        /* Otherwise the mask must be all‑ones (UINTPTR_MAX) */
        assert(result == UINTPTR_MAX);
    }

    /* 4. The result must always be either 0 or UINTPTR_MAX */
    assert(result == 0 || result == UINTPTR_MAX);
}
