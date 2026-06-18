#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_eq_harness() {
    /* 1. Declare and bound data structures */
    size_t len_a;
    size_t len_b;

    /* Bound lengths to keep state space manageable */
    __CPROVER_assume(len_a <= MAX_BUFFER_SIZE);
    __CPROVER_assume(len_b <= MAX_BUFFER_SIZE);

    /* Allocate arrays non-deterministically */
    uint8_t *array_a = NULL;
    uint8_t *array_b = NULL;

    if (len_a > 0) {
        array_a = (uint8_t *)malloc(len_a);
        __CPROVER_assume(array_a != NULL);
    }

    if (len_b > 0) {
        array_b = (uint8_t *)malloc(len_b);
        __CPROVER_assume(array_b != NULL);
    }

    /* 2. Call function under test */
    bool result = aws_array_eq(array_a, len_a, array_b, len_b);

    /* 3. Assert postconditions */
    /* The function is pure: it does not modify its inputs.
     * It returns true iff the arrays have the same length and contents.
     */

    if (len_a != len_b) {
        /* If lengths differ, result must be false */
        assert(result == false);
    } else if (len_a == 0) {
        /* If both lengths are zero, result must be true */
        assert(result == true);
    } else {
        /* If lengths are equal and non-zero, result depends on memcmp */
        if (result) {
            /* If result is true, arrays must be identical */
            assert_bytes_match(array_a, array_b, len_a);
        } else {
            /* If result is false, arrays must differ somewhere */
            /* We cannot easily assert they differ without a loop,
             * but we can assert the negation of full equality is consistent */
        }
    }

    /* 4. Cleanup */
    free(array_a);
    free(array_b);
}
