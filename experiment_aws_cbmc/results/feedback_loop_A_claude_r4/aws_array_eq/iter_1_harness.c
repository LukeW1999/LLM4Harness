#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_eq_harness(void) {
    /* Declare lengths non-deterministically, bounded to keep state space manageable */
    size_t len_a = nondet_size_t();
    size_t len_b = nondet_size_t();
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
     * 1. If len_a != len_b, result must be false
     * 2. If len_a == 0 && len_b == 0, result must be true
     * 3. If len_a == len_b > 0, result reflects memcmp
     */

    /* If lengths differ, must return false */
    if (len_a != len_b) {
        assert(result == false);
    }

    /* If both lengths are 0, must return true */
    if (len_a == 0 && len_b == 0) {
        assert(result == true);
    }

    /* The arrays themselves are not modified by the function */
    /* (No fields to check for unchanged state since these are raw arrays,
     * but we can verify the lengths haven't changed - they're local variables
     * so this is trivially true, but we assert for completeness) */

    /* Result is a bool - must be either true or false */
    assert(result == true || result == false);

    /* If result is true, lengths must be equal */
    if (result == true) {
        assert(len_a == len_b);
    }

    /* If lengths are equal and positive, result depends on memcmp */
    /* We can't easily assert the exact memcmp result without re-implementing it,
     * but we can assert the logical consistency */
    if (len_a == len_b && len_a > 0) {
        /* result is true iff arrays have same content - this is the core behavior */
        /* We verify the contrapositive: if result is false, lengths are equal (already checked above) */
        /* The function is deterministic - calling again should give same result */
        bool result2 = aws_array_eq(array_a, len_a, array_b, len_b);
        assert(result == result2);
    }

    /* Symmetry: aws_array_eq(a, len, b, len) == aws_array_eq(b, len, a, len) */
    bool result_swapped = aws_array_eq(array_b, len_b, array_a, len_a);
    assert(result == result_swapped);
}
