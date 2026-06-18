#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <aws/common/byte_buf.h>          /* provides aws_nospec_mask declaration */
#include <proof_helpers/nondet.h>

void aws_nospec_mask_harness(void) {
    /* 1. Non‑deterministic inputs */
    size_t index = nondet_size_t();
    size_t bound = nondet_size_t();

    /* 2. Call the function under test */
    size_t result = aws_nospec_mask(index, bound);

    /* 3. Post‑condition: result matches the specification */
    bool in_range = !(index >= bound) &&
                    !(bound > (SIZE_MAX / 2)) &&
                    !(index > (SIZE_MAX / 2));

    if (in_range) {
        assert(result == UINTPTR_MAX);
    } else {
        assert(result == 0);
    }

    /* 4. The result must always be either 0 or all‑ones */
    assert(result == 0 || result == UINTPTR_MAX);
}
