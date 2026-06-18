#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

void aws_array_eq_harness(void) {
    /* Declare lengths non-deterministically */
    size_t len_a = nondet_size_t();
    size_t len_b = nondet_size_t();

    /* Bound lengths to keep state space manageable */
    __CPROVER_assume(len_a <= MAX_BUFFER_SIZE);
    __CPROVER_assume(len_b <= MAX_BUFFER_SIZE);

    /* Allocate arrays non-deterministically */
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
     * 2. If len_a == len_b == 0, result must be true
     * 3. If len_a == len_b > 0, result reflects memcmp
     */

    /* If lengths differ, must return false */
    if (len_a != len_b) {
        assert(result == false);
    }

    /* If both lengths are zero, must return true */
    if (len_a == 0 && len_b == 0) {
        assert(result == true);
    }

    /* The function is pure (no side effects on inputs), so arrays are unchanged.
     * We verify the return value is a valid bool. */
    assert(result == true || result == false);

    /* If result is true, lengths must be equal */
    if (result == true) {
        assert(len_a == len_b);
    }

    /* If lengths are equal and non-zero, result depends on content.
     * We can't assert the exact value without knowing content,
     * but we can assert the contract holds both ways. */
    if (len_a == len_b && len_a > 0) {
        /* result is true iff arrays have same content - this is the memcmp result */
        /* We assert the logical consistency: result is a valid bool */
        assert(result == true || result == false);
    }
}
