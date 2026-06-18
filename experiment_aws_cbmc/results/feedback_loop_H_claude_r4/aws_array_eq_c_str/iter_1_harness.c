#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 8
#endif

void aws_array_eq_c_str_harness(void) {
    /* 1. Set up the array */
    size_t array_len;
    __CPROVER_assume(array_len <= MAX_BUFFER_SIZE);

    uint8_t *array = NULL;
    if (array_len > 0) {
        array = malloc(array_len);
        __CPROVER_assume(array != NULL);
    }

    /* 2. Set up the c_str (null-terminated string) */
    /* c_str length is non-deterministic but bounded */
    size_t c_str_len;
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);

    /* Allocate c_str_len + 1 bytes to hold the string plus null terminator */
    char *c_str = malloc(c_str_len + 1);
    __CPROVER_assume(c_str != NULL);
    /* Ensure null terminator is present */
    c_str[c_str_len] = '\0';

    /* Precondition: array may be NULL only if array_len == 0 */
    __CPROVER_assume(array != NULL || array_len == 0);

    /* 3. Call the function under test */
    bool result = aws_array_eq_c_str(array, array_len, c_str);

    /* 4. Assert postconditions */

    /* If result is true, then:
     * - array_len must equal strlen(c_str) (i.e., c_str_len)
     * - all bytes in array must match the corresponding bytes in c_str
     * - c_str must not contain a null terminator within the first array_len bytes
     */
    if (result) {
        /* The lengths must match */
        assert(array_len == c_str_len);

        /* All bytes must match */
        for (size_t i = 0; i < array_len; i++) {
            assert(array[i] == (uint8_t)c_str[i]);
            /* No null terminator within the array portion */
            assert(c_str[i] != '\0');
        }

        /* The character at position array_len must be null terminator */
        assert(c_str[array_len] == '\0');
    } else {
        /* On false result, either:
         * - lengths differ (c_str has a '\0' before array_len, or after)
         * - some byte differs
         * We can't assert much specifically, but we can verify the function
         * is consistent: if array_len == c_str_len and all bytes match,
         * result should have been true.
         *
         * We assert the contrapositive: if result is false, then either
         * the lengths differ or some byte differs.
         */

        /* Check if lengths match */
        bool lengths_match = (array_len == c_str_len);

        /* Check if all bytes match (only meaningful if lengths match) */
        bool bytes_match = true;
        if (lengths_match) {
            for (size_t i = 0; i < array_len; i++) {
                if (array[i] != (uint8_t)c_str[i] || c_str[i] == '\0') {
                    bytes_match = false;
                    break;
                }
            }
        }

        /* If lengths match and all bytes match (and no embedded nulls),
         * then result should have been true — contradiction with result==false */
        if (lengths_match && bytes_match) {
            /* This means result should be true, so we assert false here
             * only if we can confirm the null terminator is at the right place.
             * Since c_str[c_str_len] == '\0' is guaranteed above, and
             * lengths_match means array_len == c_str_len, this is consistent. */
            assert(!result == !(lengths_match && bytes_match));
        }
    }

    /* 5. The function is pure (no side effects on inputs) — nothing to check for unchanged fields */

    /* 6. Result is a bool — must be true or false */
    assert(result == true || result == false);
}
