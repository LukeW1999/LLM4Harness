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

    /* 2. Call the function under test */
    size_t result = aws_nospec_mask(index, bound);

    /* 3. Post‑condition: result must be either 0 or all‑ones */
    assert(result == 0 || result == UINTPTR_MAX);

    /* 4. Specification‑derived condition */
    bool zero_cond = (index >= bound) ||
                     (bound > (SIZE_MAX / 2)) ||
                     (index > (SIZE_MAX / 2));

    /* 5. Assert that the result matches the specification */
    if (zero_cond) {
        assert(result == 0);
    } else {
        assert(result == UINTPTR_MAX);
    }
}
