#include <aws/common/byte_buf.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_nospec_mask_harness() {
    /* 1. Declare nondeterministic inputs */
    size_t index = nondet_size_t();
    size_t bound = nondet_size_t();

    /* 2. Call the function under test */
    size_t result = aws_nospec_mask(index, bound);

    /* 3. Postcondition: result matches specification */
    bool condition = (index >= bound) ||
                     (bound > (SIZE_MAX / 2)) ||
                     (index > (SIZE_MAX / 2));

    if (condition) {
        assert(result == 0);
    } else {
        assert(result == UINTPTR_MAX);
    }
}
