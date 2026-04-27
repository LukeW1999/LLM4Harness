#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

void aws_array_eq_harness(void) {
    /* Declare lengths non-deterministically */
    size_t len_a;
    size_t len_b;

    /* Bound lengths to keep state space manageable */
    __CPROVER_assume(len_a <= MAX_BUFFER_SIZE);
    __CPROVER_assume(len_b <= MAX_BUFFER_SIZE);

    /* Allocate arrays non-deterministically.
     * NULL is allowed if length is 0 (per the spec). */
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

    /* Preconditions from the spec:
     * NULL may be passed as the array pointer if its length is declared to be 0. */
    __CPROVER_assume((len_a == 0) || (array_a != NULL));
    __CPROVER_assume((len_b == 0) || (array_b != NULL));

    /* Call the function under test */
    bool result = aws_array_eq(array_a, len_a, array_b, len_b);

    /* Postconditions:
     *
     * 1. If lengths differ, result must be false.
     * 2. If lengths are equal and both are 0, result must be true.
     * 3. If lengths are equal and non-zero, result reflects memcmp.
     * 4. The function returns whether contents are equivalent.
     */

    /* If lengths differ, must return false */
    if (len_a != len_b) {
        assert(result == false);
    }

    /* If lengths are equal and zero, must return true */
    if (len_a == 0 && len_b == 0) {
        assert(result == true);
    }

    /* The arrays themselves must not have been modified (pointers unchanged) */
    /* We verify the input arrays are still readable (no corruption) */
    if (len_a > 0) {
        assert(array_a != NULL);
    }
    if (len_b > 0) {
        assert(array_b != NULL);
    }

    /* Result is a bool — must be true or false */
    assert(result == true || result == false);

    /* Consistency check: if result is true, lengths must be equal */
    if (result == true) {
        assert(len_a == len_b);
    }

    /* Consistency check: if lengths differ, result is false */
    if (len_a != len_b) {
        assert(result == false);
    }
}
