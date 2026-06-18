#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

/* Harness for aws_nospec_mask */
void aws_nospec_mask_harness(void) {
    /* 1. Non‑deterministic inputs, bounded to keep the state space finite */
    size_t index = nondet_size_t();
    __CPROVER_assume(index <= MAX_BUFFER_SIZE);

    size_t bound = nondet_size_t();
    __CPROVER_assume(bound <= MAX_BUFFER_SIZE);

    /* 2. Call the function under test */
    size_t result = aws_nospec_mask(index, bound);

    /* 3. Expected behaviour according to the specification */
    size_t expected;
    if (index >= bound ||
        bound > (SIZE_MAX / 2) ||
        index > (SIZE_MAX / 2)) {
        expected = 0;
    } else {
        expected = UINTPTR_MAX;
    }

    /* 4. Assert that the implementation matches the specification */
    assert(result == expected);
}
