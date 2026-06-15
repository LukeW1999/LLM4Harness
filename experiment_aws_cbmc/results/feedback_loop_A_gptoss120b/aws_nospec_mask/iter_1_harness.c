#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "aws/common/byte_buf.h"   /* contains the declaration of aws_nospec_mask */

/* Non‑deterministic generators provided by the proof helpers */
size_t nondet_size_t(void);

void aws_nospec_mask_harness(void) {
    /* 1. Non‑deterministic inputs */
    size_t index = nondet_size_t();
    size_t bound = nondet_size_t();

    /* 2. Call the function under test */
    size_t mask = aws_nospec_mask(index, bound);

    /* 3. Post‑condition: mask must be either 0 or all‑ones */
    assert(mask == 0 || mask == UINTPTR_MAX);

    /* 4. Detailed specification derived from the Doxygen comment */
    bool condition = (index >= bound) ||
                     (bound > (SIZE_MAX / 2)) ||
                     (index > (SIZE_MAX / 2));

    if (condition) {
        /* When any of the out‑of‑range conditions hold, the function returns 0 */
        assert(mask == 0);
    } else {
        /* Otherwise it returns a mask with all bits set */
        assert(mask == UINTPTR_MAX);
    }
}
