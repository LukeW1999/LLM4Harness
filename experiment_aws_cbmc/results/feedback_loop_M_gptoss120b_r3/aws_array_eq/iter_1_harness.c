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
    /* 1. nondet lengths bounded */
    size_t len_a = nondet_size_t();
    __CPROVER_assume(len_a <= MAX_BUFFER_SIZE);
    size_t len_b = nondet_size_t();
    __CPROVER_assume(len_b <= MAX_BUFFER_SIZE);

    /* 2. allocate buffers if length > 0, otherwise allow NULL */
    uint8_t *array_a = NULL;
    uint8_t *array_b = NULL;

    if (len_a > 0) {
        array_a = malloc(len_a);
        __CPROVER_assume(array_a != NULL);
        /* make the contents nondeterministic */
        for (size_t i = 0; i < len_a; ++i) {
            array_a[i] = nondet_uint8_t();
        }
    }

    if (len_b > 0) {
        array_b = malloc(len_b);
        __CPROVER_assume(array_b != NULL);
        for (size_t i = 0; i < len_b; ++i) {
            array_b[i] = nondet_uint8_t();
        }
    }

    /* 3. Save a byte from each array (if any) to check immutability */
    struct store_byte_from_buffer a_store;
    struct store_byte_from_buffer b_store;
    if (len_a > 0) {
        save_byte_from_array(array_a, len_a, &a_store);
    }
    if (len_b > 0) {
        save_byte_from_array(array_b, len_b, &b_store);
    }

    /* 4. Call function under test */
    bool result = aws_array_eq(array_a, len_a, array_b, len_b);

    /* 5. Postcondition: result matches the specification */
    bool expected;
    if (len_a != len_b) {
        expected = false;
    } else if (len_a == 0) {
        expected = true;
    } else {
        expected = (memcmp(array_a, array_b, len_a) == 0);
    }
    assert(result == expected);

    /* 6. Immutability: input arrays must not be modified */
    if (len_a > 0) {
        assert_byte_from_buffer_matches(array_a, &a_store);
    }
    if (len_b > 0) {
        assert_byte_from_buffer_matches(array_b, &b_store);
    }

    /* 7. Clean up */
    free(array_a);
    free(array_b);
}
