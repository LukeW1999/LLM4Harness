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
    /* 1. Declare and bound scalar inputs */
    size_t len_a = nondet_size_t();
    __CPROVER_assume(len_a <= MAX_BUFFER_SIZE);
    size_t len_b = nondet_size_t();
    __CPROVER_assume(len_b <= MAX_BUFFER_SIZE);

    /* 2. Allocate buffers (may be NULL when length is zero) */
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

    /* 3. Save old state for immutability checks */
    struct store_byte_from_buffer old_a, old_b;
    if (len_a > 0) {
        save_byte_from_array(array_a, len_a, &old_a);
    }
    if (len_b > 0) {
        save_byte_from_array(array_b, len_b, &old_b);
    }

    /* 4. Call function under test */
    bool result = aws_array_eq(array_a, len_a, array_b, len_b);

    /* 5. Post‑condition assertions */
    if (result) {
        /* Success: lengths must be equal and contents identical */
        assert(len_a == len_b);
        if (len_a == 0) {
            /* both zero length – nothing more to check */
        } else {
            assert_bytes_match(array_a, array_b, len_a);
        }
    } else {
        /* Failure: either lengths differ or contents differ */
        assert(!(len_a == len_b && (len_a == 0 || !memcmp(array_a, array_b, len_a))));
    }

    /* 6. Immutability: input buffers must not be modified */
    if (len_a > 0) {
        assert_byte_from_buffer_matches(array_a, &old_a);
    }
    if (len_b > 0) {
        assert_byte_from_buffer_matches(array_b, &old_b);
    }

    /* 7. Clean up */
    free(array_a);
    free(array_b);
}
