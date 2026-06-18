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
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

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

    /* 2. Assume preconditions required by the function */
    __CPROVER_assume((len_a == 0) || AWS_MEM_IS_READABLE(array_a, len_a));
    __CPROVER_assume((len_b == 0) || AWS_MEM_IS_READABLE(array_b, len_b));

    /* 3. Save old state for immutability checks */
    const uint8_t *old_array_a = array_a;
    const uint8_t *old_array_b = array_b;
    const size_t old_len_a = len_a;
    const size_t old_len_b = len_b;

    /* 4. Call function under test */
    bool result = aws_array_eq(array_a, len_a, array_b, len_b);

    /* 5. Postconditions for success path */
    if (result) {
        /* lengths must be equal */
        assert(len_a == len_b);
        /* if length is non‑zero, the memory contents must be identical */
        if (len_a != 0) {
            assert(memcmp(array_a, array_b, len_a) == 0);
        }
    } else {
        /* result false implies either lengths differ or contents differ (when lengths equal and non‑zero) */
        assert(!(len_a == len_b && (len_a == 0 || memcmp(array_a, array_b, len_a) == 0)));
    }

    /* 6. Unchanged fields (function has no side effects) */
    assert(array_a == old_array_a);
    assert(array_b == old_array_b);
    assert(len_a == old_len_a);
    assert(len_b == old_len_b);

    /* 7. No additional validity invariants for raw arrays, but ensure allocated memory is still readable */
    __CPROVER_assume((len_a == 0) || AWS_MEM_IS_READABLE(array_a, len_a));
    __CPROVER_assume((len_b == 0) || AWS_MEM_IS_READABLE(array_b, len_b));
}
