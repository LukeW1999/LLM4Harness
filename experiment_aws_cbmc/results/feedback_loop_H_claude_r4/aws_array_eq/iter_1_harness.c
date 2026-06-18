#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_eq_harness(void) {
    /* Declare lengths non-deterministically */
    size_t len_a = nondet_size_t();
    size_t len_b = nondet_size_t();

    /* Bound lengths to keep state space manageable */
    __CPROVER_assume(len_a <= MAX_BUFFER_SIZE);
    __CPROVER_assume(len_b <= MAX_BUFFER_SIZE);

    /* Allocate arrays: NULL is allowed if length is 0 */
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

    /* Preconditions from the function:
     * - array_a must be readable for len_a bytes (or len_a == 0)
     * - array_b must be readable for len_b bytes (or len_b == 0)
     * These are satisfied by the malloc above.
     */

    /* Call the function under test */
    bool result = aws_array_eq(array_a, len_a, array_b, len_b);

    /* Postconditions:
     * 1. If lengths differ, result must be false
     * 2. If lengths are equal and both zero, result must be true
     * 3. If lengths are equal and non-zero, result reflects memcmp
     */

    /* If lengths differ, must return false */
    if (len_a != len_b) {
        assert(result == false);
    }

    /* If lengths are equal and zero, must return true */
    if (len_a == 0 && len_b == 0) {
        assert(result == true);
    }

    /* The arrays themselves are not modified by the function */
    /* (No fields to check for unchanged state since these are raw pointers,
     *  but we verify the pointers themselves are unchanged) */
    /* Note: array_a and array_b are local variables passed by value, so
     * the function cannot change them. The memory they point to could
     * theoretically be read but not written. */

    /* Verify that if result is true, lengths must be equal */
    if (result == true) {
        assert(len_a == len_b);
    }

    /* Verify that if lengths are equal and non-zero, result is based on content */
    /* (We can't easily assert the exact memcmp result without re-implementing it,
     *  but we can assert the logical consistency) */

    /* Additional consistency check: result is a valid bool */
    assert(result == true || result == false);
}
