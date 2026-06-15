#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_nospec_mask_harness() {
    size_t index = nondet_size_t();
    size_t bound = nondet_size_t();

    __CPROVER_assume(index <= SIZE_MAX / 2);
    __CPROVER_assume(bound <= SIZE_MAX / 2);

    size_t result = aws_nospec_mask(index, bound);

    if (index >= bound || bound > (SIZE_MAX / 2) || index > (SIZE_MAX / 2)) {
        assert(result == 0);
    } else {
        assert(result == UINTPTR_MAX);
    }
}
