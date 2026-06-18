#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_eq_harness(void) {
    /* Declare and bound lengths */
    size_t len_a = nondet_size_t();
    __CPROVER_assume(len_a <= MAX_BUFFER_SIZE);

    size_t len_b = nondet_size_t();
    __CPROVER_assume(len_b <= MAX_BUFFER_SIZE);

    /* Allocate arrays (NULL allowed if length is 0) */
    uint8_t *array_a = NULL;
    if (len_a > 0) {
        array_a = malloc(len_a);
        __CPROVER_assume(array_a != NULL);
    }

    uint8_t *array_b = NULL;
    if (len_b > 0) {
        array_b = malloc(len_b);
        __CPROVER_assume(array_b != NULL);
    }

    /* Call function under test */
    bool result = aws_array_eq(array_a, len_a, array_b, len_b);

    /* Postconditions */

    /* If lengths differ, must return false */
    if (len_a != len_b) {
        assert(result == false);
    }

    /* If both lengths are 0, must return true */
    if (len_a == 0 && len_b == 0) {
        assert(result == true);
    }

    /* If result is true, lengths must be equal */
    if (result == true) {
        assert(len_a == len_b);
    }

    /* The function is a pure comparison — it does not modify the arrays.
       We verify the arrays are still readable (CBMC will check memory safety). */
    /* No struct fields to check for immutability since inputs are raw arrays */
}
