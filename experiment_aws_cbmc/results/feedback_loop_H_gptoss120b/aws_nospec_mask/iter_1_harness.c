#include <stddef.h>
#include <stdint.h>
#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_nospec_mask_harness(void) {
    /* 1. Non‑deterministic inputs */
    size_t index = nondet_size_t();
    size_t bound = nondet_size_t();

    /* 2. Call the function under test */
    size_t result = aws_nospec_mask(index, bound);

    /* 3. Post‑condition: result must be 0 when out‑of‑range, otherwise all‑ones */
    if (index >= bound || bound > (SIZE_MAX / 2) || index > (SIZE_MAX / 2)) {
        assert(result == 0);
    } else {
        assert(result == UINTPTR_MAX);
    }

    /* 4. General sanity check: result is either 0 or all‑ones */
    assert(result == 0 || result == UINTPTR_MAX);
}
