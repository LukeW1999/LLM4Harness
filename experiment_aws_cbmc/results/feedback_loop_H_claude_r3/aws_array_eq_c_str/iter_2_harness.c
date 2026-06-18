#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
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
    size_t c_str_len;
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE + 1);

    char *c_str = malloc(c_str_len + 1); /* +1 for null terminator */
    __CPROVER_assume(c_str != NULL);
    /* Ensure null terminator is present */
    c_str[c_str_len] = '\0';

    /* 3. Preconditions from the function:
       - array may be NULL only if array_len == 0 */
    __CPROVER_assume(array != NULL || array_len == 0);

    /* 4. Call the function under test */
    bool result = aws_array_eq_c_str(array, array_len, c_str);

    /* 5. Assert postconditions */

    /* If result is true, then c_str must be null-terminated exactly at array_len */
    if (result) {
        assert(c_str[array_len] == '\0');

        /* All bytes must match */
        for (size_t i = 0; i < array_len; ++i) {
            assert(((const uint8_t *)array)[i] == (uint8_t)c_str[i]);
            /* No null terminator within the first array_len bytes of c_str */
            assert(c_str[i] != '\0');
        }
    }

    /* Verify the result is a valid boolean */
    assert(result == true || result == false);

    /* If result is true and array_len == 0, then c_str must be empty */
    if (result && array_len == 0) {
        assert(c_str[0] == '\0');
    }
}
