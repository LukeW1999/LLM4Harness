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

    /* 2. Save old state of the buffers for immutability checks */
    struct store_byte_from_buffer storage_a;
    struct store_byte_from_buffer storage_b;
    if (len_a > 0) {
        save_byte_from_array(array_a, len_a, &storage_a);
    }
    if (len_b > 0) {
        save_byte_from_array(array_b, len_b, &storage_b);
    }

    /* 3. Call function under test */
    bool result = aws_array_eq(array_a, len_a, array_b, len_b);

    /* 4. Post‑condition checks for both success and failure paths */
    if (result) {
        /* lengths must be equal */
        assert(len_a == len_b);
        /* if length > 0 the contents must be identical */
        if (len_a > 0) {
            assert_bytes_match(array_a, array_b, len_a);
        }
    } else {
        /* on failure either lengths differ or contents differ (when lengths equal and >0) */
        if (len_a == len_b && len_a > 0) {
            /* at least one byte differs – we cannot pinpoint which, but we know the arrays are not identical */
            /* The saved byte from each array cannot both be equal for all possible cases, but we assert that the
               whole buffers are not byte‑wise equal using the negation of the equality condition. */
            assert(!memcmp(array_a, array_b, len_a));
        } else {
            /* lengths differ – nothing further to assert */
            assert(len_a != len_b);
        }
    }

    /* 5. Immutability: the input buffers must not be modified */
    if (len_a > 0) {
        assert_byte_from_buffer_matches(array_a, &storage_a);
    }
    if (len_b > 0) {
        assert_byte_from_buffer_matches(array_b, &storage_b);
    }

    /* 6. Clean up */
    free(array_a);
    free(array_b);
}
