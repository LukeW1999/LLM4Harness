#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include <proof_helpers/nondet.h>

void aws_nospec_mask_harness(void) {
    /* 1. Non‑deterministic inputs */
    size_t index = nondet_size_t();
    size_t bound = nondet_size_t();

    /* 2. Call the function under test */
    size_t result = aws_nospec_mask(index, bound);

    /* 3. Post‑condition: result matches the specification */
    bool condition = (index >= bound) ||
                     (bound > (SIZE_MAX / 2)) ||
                     (index > (SIZE_MAX / 2));

    if (condition) {
        assert(result == 0);
    } else {
        assert(result == UINTPTR_MAX);
    }

    /* 4. General invariant: result is either 0 or all‑ones */
    assert(result == 0 || result == UINTPTR_MAX);
}
