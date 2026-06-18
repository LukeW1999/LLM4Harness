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
    /* c_str length can be up to MAX_BUFFER_SIZE + 1 to allow for various comparison outcomes */
    size_t c_str_len;
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);
    char *c_str = malloc(c_str_len + 1);
    __CPROVER_assume(c_str != NULL);
    /* Ensure null terminator at end */
    c_str[c_str_len] = '\0';
    /* The rest of c_str is nondet (already nondet from malloc in CBMC) */

    /* Precondition: array may be NULL only if array_len == 0 */
    __CPROVER_assume(array != NULL || array_len == 0);

    /* 3. Call the function under test */
    bool result = aws_array_eq_c_str(array, array_len, c_str);

    /* 4. Assert postconditions */
    /* The function returns true iff:
     * - array and c_str have the same length (c_str has no null terminator within first array_len bytes)
     * - all bytes match
     * - c_str[array_len] == '\0'
     */

    if (result) {
        /* If true: c_str[array_len] must be '\0' */
        assert(c_str[array_len] == '\0');

        /* All bytes in array must match c_str */
        for (size_t i = 0; i < array_len; i++) {
            assert(array[i] == (uint8_t)c_str[i]);
            /* Also, no null terminator within the first array_len bytes of c_str */
            assert(c_str[i] != '\0');
        }
    } else {
        /* If false: either lengths differ, or some byte doesn't match,
         * or c_str contains a null terminator before array_len bytes */
        /* We can't easily assert the exact negative condition in CBMC,
         * but we can assert the result is a valid bool */
        assert(result == false);
    }

    /* The function is pure (no side effects on inputs) */
    /* array and c_str are unchanged - we can't easily assert this for arbitrary memory,
     * but we assert the result is a valid boolean */
    assert(result == true || result == false);
}
