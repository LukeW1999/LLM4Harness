#include <aws/common/byte_buf.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

void aws_nospec_mask_harness(void) {
    /* 1. Non‑deterministic inputs */
    size_t index = nondet_size_t();
    size_t bound = nondet_size_t();

    /* 2. Call the function under test */
    size_t result = aws_nospec_mask(index, bound);

    /* 3. Post‑condition: result must be either 0 or all‑ones */
    assert(result == 0 || result == UINTPTR_MAX);

    /* 4. Detailed specification derived from the Doxygen comment */
    if (index >= bound || bound > (SIZE_MAX / 2) || index > (SIZE_MAX / 2)) {
        /* Out‑of‑bounds or too large: mask must be zero */
        assert(result == 0);
    } else {
        /* In‑range: mask must be all‑ones */
        assert(result == UINTPTR_MAX);
    }
}
