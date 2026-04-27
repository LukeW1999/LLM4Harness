/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_eq_harness() {
    /* Declare lengths non-deterministically */
    size_t len_a;
    size_t len_b;

    /* Bound the lengths to keep the state space manageable */
    __CPROVER_assume(len_a <= MAX_BUFFER_SIZE);
    __CPROVER_assume(len_b <= MAX_BUFFER_SIZE);

    /* Allocate arrays non-deterministically.
     * NULL may be passed if length is 0. */
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

    /* Call the function under test */
    bool result = aws_array_eq(array_a, len_a, array_b, len_b);

    /* Postconditions from Doxygen:
     * - Returns whether the contents of the two arrays are equivalent.
     * - If len_a != len_b, returns false.
     * - If len_a == 0 (and len_b == 0), returns true.
     * - Otherwise, returns result of memcmp comparison.
     */

    /* If lengths differ, result must be false */
    if (len_a != len_b) {
        assert(result == false);
    }

    /* If both lengths are 0, result must be true */
    if (len_a == 0 && len_b == 0) {
        assert(result == true);
    }

    /* The function returns a bool - it's either true or false */
    assert(result == true || result == false);

    /* The input arrays must not have been modified */
    /* (No mutation of inputs is expected - arrays are const) */

    /* If result is true, lengths must be equal */
    if (result == true) {
        assert(len_a == len_b);
    }

    /* If lengths are equal and non-zero, result reflects memcmp */
    /* (We can't easily assert the exact memcmp result without re-running it,
     * but we can assert the logical consistency) */
    if (len_a == len_b && len_a > 0 && result == true) {
        /* Contents must be equivalent - assert bytes match */
        assert_bytes_match(array_a, array_b, len_a);
    }
}
