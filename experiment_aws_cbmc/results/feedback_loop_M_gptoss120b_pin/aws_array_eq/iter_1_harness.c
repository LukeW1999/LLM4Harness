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
    /* 1. Declare and bound scalar inputs */
    size_t len_a = nondet_size_t();
    __CPROVER_assume(len_a <= MAX_BUFFER_SIZE);
    size_t len_b = nondet_size_t();
    __CPROVER_assume(len_b <= MAX_BUFFER_SIZE);

    /* 2. Allocate buffers (may be NULL when length is zero) */
    const uint8_t *array_a = NULL;
    const uint8_t *array_b = NULL;

    if (len_a > 0) {
        array_a = malloc(len_a);
        __CPROVER_assume(array_a != NULL);
    }
    if (len_b > 0) {
        array_b = malloc(len_b);
        __CPROVER_assume(array_b != NULL);
    }

    /* 3. Save old state of the buffers */
    struct store_byte_from_buffer store_a;
    struct store_byte_from_buffer store_b;
    if (len_a > 0) {
        save_byte_from_array(array_a, len_a, &store_a);
    }
    if (len_b > 0) {
        save_byte_from_array(array_b, len_b, &store_b);
    }

    /* 4. Call function under test */
    bool result = aws_array_eq(array_a, len_a, array_b, len_b);

    /* 5. Compute expected result according to specification */
    bool expected;
    if (len_a != len_b) {
        expected = false;
    } else if (len_a == 0) {
        expected = true;
    } else {
        expected = (memcmp(array_a, array_b, len_a) == 0);
    }

    /* 6. Assert postconditions */
    assert(result == expected);                     /* return value matches spec */
    if (len_a > 0) {
        assert_byte_from_buffer_matches(array_a, &store_a);   /* array_a unchanged */
    }
    if (len_b > 0) {
        assert_byte_from_buffer_matches(array_b, &store_b);   /* array_b unchanged */
    }

    /* 7. Clean up */
    free((void *)array_a);
    free((void *)array_b);
}
