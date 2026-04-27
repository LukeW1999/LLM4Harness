#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 8
#endif

void aws_array_eq_harness(void) {
    /* 1. Declare and set up array_a */
    size_t len_a;
    __CPROVER_assume(len_a <= MAX_BUFFER_SIZE);

    uint8_t *array_a = NULL;
    if (len_a > 0) {
        array_a = malloc(len_a);
        __CPROVER_assume(array_a != NULL);
    }

    /* 2. Declare and set up array_b */
    size_t len_b;
    __CPROVER_assume(len_b <= MAX_BUFFER_SIZE);

    uint8_t *array_b = NULL;
    if (len_b > 0) {
        array_b = malloc(len_b);
        __CPROVER_assume(array_b != NULL);
    }

    /* 3. Preconditions: arrays must be readable if non-zero length */
    __CPROVER_assume((len_a == 0) || (array_a != NULL));
    __CPROVER_assume((len_b == 0) || (array_b != NULL));

    /* 4. Call function under test */
    bool result = aws_array_eq(array_a, len_a, array_b, len_b);

    /* 5. Assert postconditions */

    /* If lengths differ, must return false */
    if (len_a != len_b) {
        assert(result == false);
    }

    /* If lengths are both zero, must return true */
    if (len_a == 0 && len_b == 0) {
        assert(result == true);
    }

    /* If result is true, lengths must be equal */
    if (result == true) {
        assert(len_a == len_b);
    }

    /* If result is true and len_a > 0, the arrays must have matching content */
    if (result == true && len_a > 0) {
        assert(array_a != NULL);
        assert(array_b != NULL);
        /* Content equality: assert bytes match */
        assert_bytes_match(array_a, array_b, len_a);
    }

    /* The function does not modify the input arrays or lengths */
    /* (No struct fields to check, but we verify the arrays are still accessible) */
    if (len_a > 0) {
        assert(array_a != NULL);
    }
    if (len_b > 0) {
        assert(array_b != NULL);
    }

    /* Result is a bool, so it must be true or false */
    assert(result == true || result == false);
}
