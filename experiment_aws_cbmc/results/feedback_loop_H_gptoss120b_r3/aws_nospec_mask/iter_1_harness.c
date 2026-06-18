#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <aws/common/byte_buf.h>   /* provides declaration of aws_nospec_mask */
#include <proof_helpers/nondet.h>
#include <limits.h>                /* for SIZE_MAX */

/* Harness for aws_nospec_mask */
void aws_nospec_mask_harness(void) {
    /* 1. Non‑deterministic inputs */
    size_t index = nondet_size_t();
    size_t bound = nondet_size_t();

    /* 2. Call the function under test */
    size_t result = aws_nospec_mask(index, bound);

    /* 3. Encode the specification as an assertion */
    if (index >= bound ||
        bound > (SIZE_MAX / 2) ||
        index > (SIZE_MAX / 2)) {
        /* Out‑of‑range case */
        assert(result == 0);
    } else {
        /* In‑range case */
        assert(result == UINTPTR_MAX);
    }
}
