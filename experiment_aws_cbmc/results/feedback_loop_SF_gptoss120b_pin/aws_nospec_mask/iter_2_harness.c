#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_nospec_mask_harness(void) {
    size_t index = nondet_size_t();
    size_t bound = nondet_size_t();

    /* Precondition assumptions (if any) */
    __CPROVER_assume(index <= SIZE_MAX);
    __CPROVER_assume(bound <= SIZE_MAX);

    /* Snapshot of inputs */
    size_t old_index = index;
    size_t old_bound = bound;

    /* Call the function under test */
    size_t result = aws_nospec_mask(index, bound);

    /* The function must not modify its inputs */
    assert(index == old_index);
    assert(bound == old_bound);

    /* The result must be either all bits set or zero */
    assert(result == 0 || result == SIZE_MAX);

    /* If the result is non‑zero then index must be less than bound */
    if (result != 0) {
        assert(index < bound);
    }
}
