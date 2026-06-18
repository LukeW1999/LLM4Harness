#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include <aws/common/byte_buf.h>          /* declares aws_nospec_mask */
#include <proof_helpers/nondet.h>         /* nondet_size_t */

void aws_nospec_mask_harness() {
    /* 1. Non‑deterministic inputs */
    size_t index = nondet_size_t();
    size_t bound = nondet_size_t();

    /* 2. Call the function under test */
    size_t result = aws_nospec_mask(index, bound);

    /* 3. Post‑condition checks derived from the Doxygen description */
    bool out_of_range = (index >= bound) ||
                        (bound > (SIZE_MAX / 2)) ||
                        (index > (SIZE_MAX / 2));

    if (out_of_range) {
        /* When any out‑of‑range condition holds, the function must return 0 */
        assert(result == 0);
    } else {
        /* Otherwise it must return all‑ones (UINTPTR_MAX) */
        assert(result == UINTPTR_MAX);
    }

    /* 4. The result is always either 0 or UINTPTR_MAX */
    assert(result == 0 || result == UINTPTR_MAX);
}
