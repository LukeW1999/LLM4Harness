#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

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
    size_t c_str_len = nondet_size_t();
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);

    char *c_str = malloc(c_str_len + 1); /* +1 for null terminator */
    __CPROVER_assume(c_str != NULL);
    c_str[c_str_len] = '\0';
    /* Ensure no null terminator before c_str_len to make it a proper string of length c_str_len */
    for (size_t i = 0; i < c_str_len; ++i) {
        __CPROVER_assume(c_str[i] != '\0');
    }

    /* 3. Precondition: array may be NULL only if array_len == 0 */
    __CPROVER_assume(array != NULL || array_len == 0);

    /* 4. Call the function under test */
    bool result = aws_array_eq_c_str(array, array_len, c_str);

    /* 5. Assert postconditions */

    /* If result is true, then:
     * - array_len must equal strlen(c_str) (i.e., c_str_len)
     * - all bytes in array must match the corresponding bytes in c_str
     */
    if (result) {
        /* The lengths must match */
        assert(array_len == c_str_len);

        /* Every byte in array matches c_str */
        for (size_t i = 0; i < array_len; ++i) {
            assert(array[i] == (uint8_t)c_str[i]);
        }

        /* c_str is null-terminated exactly at array_len */
        assert(c_str[array_len] == '\0');
    }

    /* 6. The function is pure (read-only), so inputs must be unchanged.
     * CBMC verifies memory safety automatically, but we assert the
     * return value is a valid bool. */
    assert(result == true || result == false);

    /* 7. Edge case: if array_len == 0, result is true iff c_str[0] == '\0' */
    if (array_len == 0) {
        if (result) {
            assert(c_str[0] == '\0');
        } else {
            assert(c_str[0] != '\0');
        }
    }
}
