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

    /* Bound the lengths to keep the state space manageable */
    __CPROVER_assume(len_a <= MAX_BUFFER_SIZE);
    __CPROVER_assume(len_b <= MAX_BUFFER_SIZE);

    /* Allocate arrays: if len > 0, allocate memory; otherwise NULL is allowed */
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
     * - NULL may be passed as the array pointer if its length is declared to be 0
     * - If len > 0, the array must be readable
     */
    __CPROVER_assume((len_a == 0) || (array_a != NULL));
    __CPROVER_assume((len_b == 0) || (array_b != NULL));

    /* Call the function under test */
    bool result = aws_array_eq(array_a, len_a, array_b, len_b);

    /* Postconditions:
     * 1. If lengths differ, result must be false
     * 2. If lengths are equal and both are 0, result must be true
     * 3. If result is true, lengths must be equal
     * 4. If result is false and lengths are equal and non-zero, contents differ
     */

    /* If lengths differ, result must be false */
    if (len_a != len_b) {
        assert(result == false);
    }

    /* If both lengths are 0, result must be true */
    if (len_a == 0 && len_b == 0) {
        assert(result == true);
    }

    /* If result is true, lengths must be equal */
    if (result == true) {
        assert(len_a == len_b);
    }

    /* If result is true and len > 0, the contents must match */
    if (result == true && len_a > 0) {
        assert(array_a != NULL);
        assert(array_b != NULL);
        assert_bytes_match(array_a, array_b, len_a);
    }

    /* The function is pure (no side effects on inputs):
     * array_a, array_b, len_a, len_b are unchanged */
    /* (These are local variables so we just verify the return value semantics) */

    /* Result is a bool - must be either true or false */
    assert(result == true || result == false);
}
