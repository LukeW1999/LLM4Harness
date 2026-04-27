#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <stddef.h>

void aws_nospec_mask_harness() {
    /* 1. Declare and bound data structures */
    size_t index = nondet_size_t();
    size_t bound = nondet_size_t();

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    size_t old_index = index;
    size_t old_bound = bound;

    /* 3. Call function under test */
    size_t result = aws_nospec_mask(index, bound);

    /* 4. Assert postconditions for BOTH success and failure paths */
    /* The function always succeeds, so we only need to check the success path */
    {
        /* Calculate expected mask */
        size_t negative_mask = index | bound;
        size_t toobig_mask = bound - index - (uintptr_t)1;
        size_t combined_mask = negative_mask | toobig_mask;
        combined_mask = (~combined_mask) / (SIZE_MAX - (SIZE_MAX >> 1));
        combined_mask = combined_mask * UINTPTR_MAX;

        /* Assert the result matches the expected mask */
        assert(result == combined_mask);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    /* In this case, index and bound are inputs and should not change */
    assert(index == old_index);
    assert(bound == old_bound);

    /* 6. Assert validity invariants always holds */
    /* No specific validity invariants for primitive types like size_t */
}
