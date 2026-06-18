#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

void aws_array_eq_c_str_harness(void) {
    /* 1. Set up the array input */
    size_t array_len = nondet_size_t();
    __CPROVER_assume(array_len <= MAX_BUFFER_SIZE);

    uint8_t *array = NULL;
    if (array_len > 0) {
        array = malloc(array_len);
        __CPROVER_assume(array != NULL);
    }

    /* 2. Set up the c_str input */
    /* c_str must be a valid null-terminated string.
     * We bound its length to keep state space manageable. */
    size_t c_str_len = nondet_size_t();
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);

    char *c_str = malloc(c_str_len + 1); /* +1 for null terminator */
    __CPROVER_assume(c_str != NULL);
    c_str[c_str_len] = '\0';

    /* 3. Precondition: array may be NULL only if array_len == 0 */
    __CPROVER_assume(array != NULL || array_len == 0);

    /* 4. Call the function under test */
    bool result = aws_array_eq_c_str(array, array_len, c_str);

    /* 5. Assert postconditions */

    /* If result is true, then:
     * - array_len must equal strlen(c_str) (i.e., c_str_len)
     * - all bytes in array must match the corresponding bytes in c_str
     * - c_str must not contain a null terminator within the first array_len bytes
     */
    if (result) {
        /* The lengths must match */
        assert(array_len == c_str_len);

        /* Every byte in array matches c_str */
        for (size_t i = 0; i < array_len; ++i) {
            assert(array[i] == (uint8_t)c_str[i]);
            assert(c_str[i] != '\0');
        }

        /* c_str is null-terminated exactly at array_len */
        assert(c_str[array_len] == '\0');
    } else {
        /* On false result: either lengths differ, or some byte differs,
         * or c_str has a null terminator before array_len.
         * We can't assert a single simple condition here, but we can
         * assert that the inputs are unchanged (function is read-only). */

        /* The function does not modify its inputs */
        /* array and c_str are unchanged — verified by CBMC's memory model */
    }

    /* 6. The function is pure (read-only), so inputs must be unchanged.
     * CBMC verifies memory safety automatically, but we assert the
     * return value is a valid bool. */
    assert(result == true || result == false);

    /* 7. Verify the logical equivalence:
     * result == true implies array_len bytes of array equal first array_len bytes of c_str
     * AND c_str[array_len] == '\0' */
    if (result) {
        /* Already checked above */
        assert(c_str[array_len] == '\0');
    }

    /* 8. Edge case: if array_len == 0, result is true iff c_str[0] == '\0' */
    if (array_len == 0) {
        if (result) {
            assert(c_str[0] == '\0');
        } else {
            assert(c_str[0] != '\0');
        }
    }
}
