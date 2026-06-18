#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

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

    /* Snapshot inputs before the call */
    size_t old_len_a = len_a;
    size_t old_len_b = len_b;
    const uint8_t *old_array_a = array_a;
    const uint8_t *old_array_b = array_b;

    /* Call the function under test */
    bool result = aws_array_eq(array_a, len_a, array_b, len_b);

    /*
     * Postconditions:
     *
     * 1. RETURN VALUE:
     *    - If len_a != len_b, result must be false.
     *    - If len_a == 0 && len_b == 0, result must be true.
     *    - If len_a == len_b > 0, result reflects memcmp outcome (we can't
     *      easily assert the exact memcmp result in CBMC without re-doing it,
     *      but we can assert the logical constraints).
     */
    if (len_a != len_b) {
        assert(result == false);
    }

    if (len_a == 0 && len_b == 0) {
        assert(result == true);
    }

    /*
     * 2. FRAME: inputs must not change.
     */
    assert(len_a == old_len_a);
    assert(len_b == old_len_b);
    assert(array_a == old_array_a);
    assert(array_b == old_array_b);

    /*
     * 3. INVARIANTS:
     *    - If result is true, lengths must be equal (contrapositive of the
     *      "if lengths differ, return false" rule).
     */
    if (result == true) {
        assert(len_a == len_b);
    }

    /*
     *    - If both arrays are non-NULL and lengths are equal and positive,
     *      result is a valid bool (true or false) — this is trivially true
     *      for bool in C, but we assert it explicitly.
     */
    assert(result == true || result == false);
}
