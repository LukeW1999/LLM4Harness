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
    /* nondeterministic lengths bounded by MAX_BUFFER_SIZE */
    size_t len_a = nondet_size_t();
    size_t len_b = nondet_size_t();
    __CPROVER_assume(len_a <= MAX_BUFFER_SIZE);
    __CPROVER_assume(len_b <= MAX_BUFFER_SIZE);

    /* allocate buffers according to lengths */
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

    /* Save old state for immutability checks */
    struct store_byte_from_buffer old_a = {0};
    struct store_byte_from_buffer old_b = {0};

    if (len_a > 0) {
        save_byte_from_array(array_a, len_a, &old_a);
    }
    if (len_b > 0) {
        save_byte_from_array(array_b, len_b, &old_b);
    }

    size_t old_len_a = len_a;
    size_t old_len_b = len_b;
    uint8_t *old_ptr_a = array_a;
    uint8_t *old_ptr_b = array_b;

    /* Call function under test */
    bool result = aws_array_eq(array_a, len_a, array_b, len_b);

    /* Postconditions: result reflects equality */
    bool expected;
    if (len_a != len_b) {
        expected = false;
    } else if (len_a == 0) {
        expected = true;
    } else {
        expected = (memcmp(array_a, array_b, len_a) == 0);
    }
    assert(result == expected);

    /* Unchanged inputs */
    assert(len_a == old_len_a);
    assert(len_b == old_len_b);
    assert(array_a == old_ptr_a);
    assert(array_b == old_ptr_b);

    if (len_a > 0) {
        assert_byte_from_buffer_matches(array_a, &old_a);
    }
    if (len_b > 0) {
        assert_byte_from_buffer_matches(array_b, &old_b);
    }

    /* Clean up */
    free(array_a);
    free(array_b);
}
