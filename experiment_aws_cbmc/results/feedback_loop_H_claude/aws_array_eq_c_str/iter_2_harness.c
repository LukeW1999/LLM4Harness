#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

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
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);

    /* Allocate c_str_len + 1 bytes to hold the string plus null terminator */
    char *c_str = malloc(c_str_len + 1);
    __CPROVER_assume(c_str != NULL);

    /* Ensure no null bytes before c_str_len, so strlen(c_str) == c_str_len */
    for (size_t i = 0; i < c_str_len; i++) {
        __CPROVER_assume(c_str[i] != '\0');
    }
    /* Ensure null terminator is present at exactly c_str_len */
    c_str[c_str_len] = '\0';

    /* Precondition: array may be NULL only if array_len == 0 */
    __CPROVER_assume(array != NULL || array_len == 0);

    /* 3. Call the function under test */
    bool result = aws_array_eq_c_str(array, array_len, c_str);

    /* 4. Assert postconditions */
    if (result) {
        /* The lengths must match: strlen(c_str) == c_str_len by construction */
        assert(array_len == c_str_len);

        /* All bytes must match */
        for (size_t i = 0; i < array_len; i++) {
            assert(array[i] == (uint8_t)c_str[i]);
            assert(c_str[i] != '\0');
        }

        /* The c_str must be null-terminated exactly at array_len */
        assert(c_str[array_len] == '\0');
    } else {
        /* Verify contrapositive: if all conditions for equality hold, result should be true */
        bool lengths_match = (c_str_len == array_len);
        bool bytes_match = true;

        for (size_t i = 0; i < array_len; i++) {
            if (array != NULL && array[i] != (uint8_t)c_str[i]) {
                bytes_match = false;
                break;
            }
        }

        /* If lengths match and all bytes match, result should have been true */
        if (lengths_match && bytes_match) {
            assert(false); /* Should not reach here if function is correct */
        }
    }

    /* Result is a bool */
    assert(result == true || result == false);
}
