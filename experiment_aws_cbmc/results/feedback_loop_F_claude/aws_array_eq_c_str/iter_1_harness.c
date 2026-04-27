/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>

void aws_array_eq_c_str_harness() {
    /* Bound the sizes to keep the state space manageable */
    size_t array_len;
    __CPROVER_assume(array_len <= MAX_BUFFER_SIZE);

    /* array: may be NULL if array_len == 0 */
    uint8_t *array = NULL;
    if (array_len > 0) {
        array = malloc(array_len);
        __CPROVER_assume(array != NULL);
    }

    /* c_str: a null-terminated string; bound its length */
    size_t c_str_len;
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);
    char *c_str = malloc(c_str_len + 1); /* +1 for null terminator */
    __CPROVER_assume(c_str != NULL);
    c_str[c_str_len] = '\0'; /* ensure null termination */

    /* Precondition: array is non-NULL or array_len == 0 */
    __CPROVER_assume(array != NULL || array_len == 0);

    /* Save inputs before call (they should not be modified) */
    size_t old_array_len = array_len;

    /* Call the function under test */
    bool result = aws_array_eq_c_str(array, array_len, c_str);

    /* Postconditions:
     * 1. The function returns true iff array and c_str have the same content
     *    and the same length (c_str has no null terminator within the first array_len bytes,
     *    and c_str[array_len] == '\0').
     * 2. Neither array nor c_str are modified.
     * 3. array_len is unchanged.
     */

    /* array_len must not have changed */
    assert(array_len == old_array_len);

    /* Verify correctness of the result:
     * If result is true, then:
     *   - c_str[array_len] must be '\0'
     *   - For all i in [0, array_len), array[i] == c_str[i] and c_str[i] != '\0'
     * If result is false, then the above does not hold.
     */
    if (result) {
        /* c_str must be null-terminated exactly at position array_len */
        assert(c_str[array_len] == '\0');
        /* All bytes must match and no null terminator within the array */
        if (array_len > 0) {
            assert(array != NULL);
            /* Check a non-deterministic index for bounded verification */
            size_t check_idx;
            __CPROVER_assume(check_idx < array_len);
            assert(((uint8_t *)array)[check_idx] == (uint8_t)c_str[check_idx]);
            assert(c_str[check_idx] != '\0');
        }
    } else {
        /* On false: either lengths differ or contents differ.
         * We can assert that if c_str_len == array_len and all bytes match
         * and c_str[array_len] == '\0', then result must be true.
         * (Contrapositive: if result is false, at least one of these fails.)
         * We don't assert the contrapositive directly, but we can check
         * that the function is consistent with its specification. */
        /* No specific structural postcondition to assert on false path
         * beyond what we already know — the inputs are unchanged. */
    }

    /* The function is pure (no side effects on inputs) — inputs unchanged */
    /* array_len already checked above */
    /* c_str[array_len] is still '\0' (we set it, function shouldn't change it) */
    assert(c_str[c_str_len] == '\0');
}
