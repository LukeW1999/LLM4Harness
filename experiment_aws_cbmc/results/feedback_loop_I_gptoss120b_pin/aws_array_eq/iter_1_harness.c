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
    /* 1. Nondeterministic inputs bounded by MAX_BUFFER_SIZE */
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

    /* Save old contents for immutability checks */
    uint8_t *old_a = NULL;
    uint8_t *old_b = NULL;
    if (len_a > 0) {
        old_a = malloc(len_a);
        __CPROVER_assume(old_a != NULL);
        memcpy(old_a, array_a, len_a);
    }
    if (len_b > 0) {
        old_b = malloc(len_b);
        __CPROVER_assume(old_b != NULL);
        memcpy(old_b, array_b, len_b);
    }

    /* 2. Call function under test */
    bool result = aws_array_eq(array_a, len_a, array_b, len_b);

    /* 3. Postconditions for success */
    if (result) {
        /* lengths must be equal */
        assert(len_a == len_b);
        /* if non‑empty, contents must be identical */
        if (len_a > 0) {
            assert_bytes_match(array_a, array_b, len_a);
        }
    } else {
        /* on failure either lengths differ or contents differ */
        if (len_a == len_b) {
            /* both zero would have succeeded, so this case cannot happen */
            assert(len_a != 0);
            /* contents must differ */
            assert(memcmp(array_a, array_b, len_a) != 0);
        } else {
            /* lengths differ – no further condition needed */
        }
    }

    /* 4. Immutability: input buffers must not be modified */
    if (len_a > 0) {
        assert_bytes_match(array_a, old_a, len_a);
    }
    if (len_b > 0) {
        assert_bytes_match(array_b, old_b, len_b);
    }

    /* 5. Clean up */
    free(array_a);
    free(array_b);
    free(old_a);
    free(old_b);
}
