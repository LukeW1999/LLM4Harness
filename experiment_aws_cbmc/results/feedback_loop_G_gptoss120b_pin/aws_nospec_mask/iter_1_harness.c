#include <aws/common/byte_buf.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_nospec_mask_harness(void) {
    /* 1. Declare nondeterministic inputs */
    size_t index = nondet_size_t();
    size_t bound = nondet_size_t();

    /* 2. Call the function under test */
    size_t result = aws_nospec_mask(index, bound);

    /* 3. Postcondition: result must be either 0 or all‑ones */
    assert(result == 0 || result == UINTPTR_MAX);

    /* 4. Detailed condition based on the specification */
    if (index >= bound || bound > (SIZE_MAX / 2) || index > (SIZE_MAX / 2)) {
        assert(result == 0);
    } else {
        assert(result == UINTPTR_MAX);
    }

    /* 5. No side‑effects on inputs (they are passed by value) */
    assert(index == index);
    assert(bound == bound);
}
