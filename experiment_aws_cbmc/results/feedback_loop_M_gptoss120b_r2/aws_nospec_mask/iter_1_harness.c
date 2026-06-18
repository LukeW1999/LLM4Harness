#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>

void aws_nospec_mask_harness(void) {
    /* 1. Non-deterministic inputs bounded to avoid UNKNOWN */
    size_t index = nondet_size_t();
    __CPROVER_assume(index <= MAX_BUFFER_SIZE);

    size_t bound = nondet_size_t();
    __CPROVER_assume(bound <= MAX_BUFFER_SIZE);

    /* 2. Call the function under test */
    size_t result = aws_nospec_mask(index, bound);

    /* 3. Expected behavior per specification */
    bool condition = (index >= bound) ||
                     (bound > (SIZE_MAX / 2)) ||
                     (index > (SIZE_MAX / 2));

    size_t expected = condition ? 0 : UINTPTR_MAX;

    /* 4. Assert that the returned mask matches the specification */
    assert(result == expected);
}
