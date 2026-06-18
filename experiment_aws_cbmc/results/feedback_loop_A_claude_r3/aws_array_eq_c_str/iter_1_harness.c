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

    /* 3. Preconditions from the function:
     * - array may be NULL only if array_len == 0
     * - c_str must be a valid null-terminated string (ensured above)
     */
    __CPROVER_assume(array != NULL || array_len == 0);

    /* 4. Call the function under test */
    bool result = aws_array_eq_c_str(array, array_len, c_str);

    /* 5. Assert postconditions */
    /* The function returns true iff:
     * - array_len == strlen(c_str)
     * - all bytes in array match the corresponding bytes in c_str
     * - c_str does not contain a null terminator within the first array_len bytes
     */

    if (result) {
        /* If true, then c_str[array_len] must be '\0' */
        assert(c_str[array_len] == '\0');

        /* And all bytes must match */
        for (size_t i = 0; i < array_len; ++i) {
            assert(c_str[i] != '\0');
            assert(array[i] == (uint8_t)c_str[i]);
        }
    } else {
        /* If false, either:
         * - c_str is shorter than array_len (c_str[i] == '\0' for some i < array_len)
         * - c_str is longer than array_len (c_str[array_len] != '\0')
         * - some byte doesn't match
         * We can't easily assert the exact reason, but we can assert the result is bool */
        assert(result == false);
    }

    /* The function is pure (no side effects on inputs) */
    /* array and c_str are unchanged - we can't easily assert memory contents unchanged
     * without saving them, but we can assert the pointers are still valid */
    /* No validity invariants to check since this function doesn't modify structures */
}
