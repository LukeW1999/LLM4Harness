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
    /* 1. Non‑deterministic lengths bounded by the proof constant */
    size_t len_a = nondet_size_t();
    size_t len_b = nondet_size_t();
    __CPROVER_assume(len_a <= MAX_BUFFER_SIZE);
    __CPROVER_assume(len_b <= MAX_BUFFER_SIZE);

    /* 2. Allocate buffers when length is non‑zero */
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

    /* 3. Save a snapshot of the original memory for immutability checks */
    struct store_byte_from_buffer old_a, old_b;
    if (len_a > 0) {
        save_byte_from_array(array_a, len_a, &old_a);
    }
    if (len_b > 0) {
        save_byte_from_array(array_b, len_b, &old_b);
    }

    /* 4. Call the function under test */
    bool result = aws_array_eq(array_a, len_a, array_b, len_b);

    /* 5. Post‑condition: result == true  ⇒  lengths equal and contents equal */
    if (result) {
        assert(len_a == len_b);
        if (len_a > 0) {
            assert_bytes_match(array_a, array_b, len_a);
        }
    } else {
        /* result == false  ⇒  either lengths differ or contents differ */
        bool lengths_equal = (len_a == len_b);
        bool contents_equal = false;
        if (lengths_equal) {
            if (len_a == 0) {
                contents_equal = true;
            } else {
                contents_equal = (memcmp(array_a, array_b, len_a) == 0);
            }
        }
        assert(!(lengths_equal && contents_equal));
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
