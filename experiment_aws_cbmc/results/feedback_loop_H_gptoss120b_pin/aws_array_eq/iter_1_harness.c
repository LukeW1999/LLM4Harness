#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <aws/common/linked_list.h>
#include <aws/common/math.h>
#include <aws/common/string.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_eq_harness(void) {
    /* 1. Declare nondeterministic inputs and bound them */
    size_t len_a = nondet_size_t();
    size_t len_b = nondet_size_t();
    __CPROVER_assume(len_a <= MAX_BUFFER_SIZE);
    __CPROVER_assume(len_b <= MAX_BUFFER_SIZE);

    uint8_t *array_a = NULL;
    uint8_t *array_b = NULL;

    if (len_a > 0) {
        array_a = malloc(len_a);
        __CPROVER_assume(array_a != NULL);
    }
    if (len_b > 0) {
        array_b = malloc(len_b);
        __CPROVER_assume(array_b != NULL);
    }

    /* 2. Assume preconditions required by the implementation */
    __CPROVER_assume((len_a == 0) || (array_a != NULL));
    __CPROVER_assume((len_b == 0) || (array_b != NULL));

    /* 3. Save old state (inputs are immutable, but we record them for clarity) */
    const void *old_array_a = array_a;
    const void *old_array_b = array_b;
    size_t old_len_a = len_a;
    size_t old_len_b = len_b;

    /* 4. Call function under test */
    bool result = aws_array_eq(array_a, len_a, array_b, len_b);

    /* 5. Post‑condition assertions */
    if (result) {
        /* Success: lengths must be equal and contents identical (or both zero) */
        assert(len_a == len_b);
        if (len_a > 0) {
            assert(memcmp(array_a, array_b, len_a) == 0);
        }
    } else {
        /* Failure: either lengths differ or, when equal and non‑zero, contents differ */
        assert(!(len_a == len_b && (len_a == 0 || memcmp(array_a, array_b, len_a) == 0)));
    }

    /* 6. Unchanged inputs */
    assert(array_a == old_array_a);
    assert(array_b == old_array_b);
    assert(len_a == old_len_a);
    assert(len_b == old_len_b);

    /* 7. Basic validity of allocated buffers */
    if (len_a > 0) {
        assert(array_a != NULL);
    }
    if (len_b > 0) {
        assert(array_b != NULL);
    }

    /* 8. Clean up */
    free(array_a);
    free(array_b);
}
